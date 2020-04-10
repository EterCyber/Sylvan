/*
    This file is part of Sylvan.
    Copyright (C) 2019-2020 Wilbert Lee

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sylvan.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <QStringList>

#include "board/board.h"
#include "board/boardfactory.h"
#include "timecontrol.h"
#include "ucciengine.h"
#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"
#include "enginetextoption.h"

namespace {

QString variantFromUcci(QString str, bool ucciPrefix = true)
{
    if (ucciPrefix)
    {
        if (!str.startsWith("UCCI_"))
            return QString();
        str = str.mid(4);
    }
    if (str.isEmpty())
        return QString();

    str = str.toLower();
    if (str == "chess960")
        str = "fischerandom";

    if (!Chess::BoardFactory::variants().contains(str))
        return QString();
    return str;
}

QString variantToUcci(const QString& str, bool ucciPrefix = true)
{
    if (str.isEmpty() || str == "standard")
        return QString();

    if (str == "fischerandom")
        return ucciPrefix ? "UCCI_Chess960" : "chess960";
    if (!ucciPrefix)
        return str;
    if (str == "caparandom")
        return "UCCI_CapaRandom";

    QString tmp = QString("UCCI_%1").arg(str);
    tmp[4] = tmp.at(4).toUpper();
    return tmp;
}

QStringRef joinTokens(const QVarLengthArray<QStringRef>& tokens)
{
    Q_ASSERT(!tokens.isEmpty());

    const QStringRef& last = tokens[tokens.size() - 1];
    int start = tokens[0].position();
    int end = last.position() + last.size();

    return QStringRef(last.string(), start, end - start);
}

} // namespace

UcciEngine::UcciEngine(QObject* parent)
    : ChessEngine(parent),
      m_useDirectPv(false),
      m_sendOpponentsName(false),
      m_canPonder(false),
      m_useMillisec(false),
      m_ponderState(NotPondering),
      m_movesPondered(0),
      m_ponderHits(0),
      m_ignoreThinking(false),
      m_rePing(false)
{
    addVariant("standard");
    setName("UcciEngine");
}

void UcciEngine::startProtocol()
{
    // Tell the engine to turn on UCCI mode
    write("ucci");
}

QString UcciEngine::positionString(bool position)
{
    QString str("");

    if (position)
        str += QString("position ");

    str += QString("fen ") + m_startFen;

    if (!m_moveStrings.isEmpty())
        str += QString(" moves") + m_moveStrings;

    return str;
}

void UcciEngine::sendPosition()
{
    write(positionString(true));
    write(positionString(false));
}

void UcciEngine::startGame()
{
    Q_ASSERT(supportsVariant(board()->variant()));
    const QList<QString> directPvList = {"giveaway", "suicide", "antichess"};

    m_ignoreThinking = false;
    m_rePing = false;
    m_ponderState = NotPondering;
    m_ponderMove = Chess::Move();
    m_ponderMoveSan.clear();
    m_movesPondered = 0;
    m_ponderHits = 0;
    m_bmBuffer.clear();
    m_moveStrings.clear();
    m_useDirectPv = directPvList.contains(board()->variant());
    m_startFen = board()->fenString(Chess::Board::XFen);
    setVariant(board()->variant());

    write("newgame");

    if (m_canPonder)
        sendOption("ponder", pondering());

    if (m_sendOpponentsName)
    {
        QString opType = opponent()->isHuman() ? "human" : "computer";
        QString value = QString("none none %1 %2")
                .arg(opType, opponent()->name());
        sendOption("UCCI_Opponent", value);
    }

    sendPosition();
}

void UcciEngine::endGame(const Chess::Result& result)
{
    m_ignoreThinking = true;
    if (stopThinking())
        ping(false);
    ChessEngine::endGame(result);
}

void UcciEngine::makeMove(const Chess::Move& move)
{
    if (!m_ponderMove.isNull())
    {
        m_movesPondered++;
        bool gotPonderHit = (move == m_ponderMove);
        if (gotPonderHit)
            m_ponderHits++;

        if (pondering())
        {
            if (gotPonderHit)
                m_ponderState = PonderHit;

            m_ponderMove = Chess::Move();
            m_ponderMoveSan.clear();
            if (m_ponderState != PonderHit)
            {
                m_moveStrings.truncate(m_moveStrings.lastIndexOf(' '));
                if (isReady())
                {
                    m_ignoreThinking = true;
                    if (stopThinking())
                        ping(false);
                }
                else
                {
                    // Cancel sending the "go ponder" message
                    clearWriteBuffer();
                    m_rePing = true;
                }
            }
        }
    }

    if (m_ponderState != PonderHit)
    {
        m_ponderState = NotPondering;
        m_moveStrings += " " + board()->moveString(move, Chess::Board::LongAlgebraic);
        if (m_ignoreThinking)
        {
            m_bmBuffer << positionString(true) << "isready";
            m_bmBuffer << positionString(false) << "isready";
        }
        else
        {
            sendPosition();
        }
    }
}

void UcciEngine::makeBookMove(const Chess::Move& move)
{
    if (stopThinking())
        ping(false);
    clearPonderState();

    ChessEngine::makeBookMove(move);
}

void UcciEngine::startThinking()
{
    if (m_ponderState == PonderHit)
    {
        m_ponderState = NotPondering;
        write("ponderhit");
        return;
    }

    const TimeControl* redTc = nullptr;
    const TimeControl* blackTc = nullptr;
    const TimeControl* myTc = timeControl();
    if (side() == Chess::Side::Red)
    {
        redTc = myTc;
        blackTc = opponent()->timeControl();
    }
    else if (side() == Chess::Side::Black)
    {
        redTc = opponent()->timeControl();
        blackTc = myTc;
    }
    else
        qFatal("Player %s doesn't have a side", qUtf8Printable(name()));

    QString command = "go";
    if (pondering() && !m_ponderMove.isNull())
    {
        command += " ponder";
        m_ponderState = Pondering;
    }
    else
        m_ponderState = NotPondering;
    if (myTc->isInfinite())
    {
        if (myTc->plyLimit() == 0 && myTc->nodeLimit() == 0)
            command += " infinite";
    }
    else if (myTc->timePerMove() > 0)
    {
        int time = myTc->timeLeft();
        if (!m_useMillisec)
            time /= 1000;
        command += QString(" time %1").arg(time);
    }
    else
    {
        command += QString(" wtime %1").arg(redTc->timeLeft());
        command += QString(" btime %1").arg(blackTc->timeLeft());
        if (redTc->timeIncrement() > 0)
            command += QString(" winc %1").arg(redTc->timeIncrement());
        if (blackTc->timeIncrement() > 0)
            command += QString(" binc %1").arg(blackTc->timeIncrement());
        if (myTc->movesLeft() > 0)
            command += QString(" movestogo %1").arg(myTc->movesLeft());
    }
    if (myTc->plyLimit() > 0)
        command += QString(" depth %1").arg(myTc->plyLimit());
    if (myTc->nodeLimit() > 0)
        command += QString(" nodes %1").arg(myTc->nodeLimit());

    write(command);
}

void UcciEngine::startPondering()
{
    if (!pondering() || m_ponderMove.isNull())
        return;

    m_moveStrings += " " + board()->moveString(m_ponderMove, Chess::Board::LongAlgebraic);
    sendPosition();
    ping();
    startThinking();
}

void UcciEngine::clearPonderState()
{
    m_ponderState = NotPondering;
    m_ponderMove = Chess::Move();
    m_ponderMoveSan.clear();
}

bool UcciEngine::isPondering() const
{
    return (m_ponderState != NotPondering);
}

void UcciEngine::sendStop()
{
    write("stop");
}

QString UcciEngine::protocol() const
{
    return "ucci";
}

bool UcciEngine::sendPing()
{
    write("isready");
    return true;
}

void UcciEngine::sendQuit()
{
    write("quit");
}

QStringRef UcciEngine::parseUcciTokens(const QStringRef& first,
                                     const QString* types,
                                     int typeCount,
                                     QVarLengthArray<QStringRef>& tokens,
                                     int& type)
{
    QStringRef token(first);
    type = -1;
    tokens.clear();

    do
    {
        bool newType = false;
        for (int i = 0; i < typeCount; i++)
        {
            if (token == types[i])
            {
                if (type != -1)
                    return token;
                type = i;
                newType = true;
                break;
            }
        }
        if (!newType && type != -1)
            tokens.append(token);
    }
    while (!(token = nextToken(token)).isNull());

    return token;
}

void UcciEngine::parseInfo(const QVarLengthArray<QStringRef>& tokens,
                          int type,
                          MoveEvaluation* eval)
{
    enum Keyword
    {
        InfoDepth,
        InfoSelDepth,
        InfoTime,
        InfoNodes,
        InfoScore,
        InfoPv,
        InfoMultiPv,
        InfoCurrMove,
        InfoCurrMoveNumber,
        InfoHashFull,
        InfoNps,
        InfoTbHits,
        InfoCpuLoad,
        InfoString,
        InfoRefutation,
        InfoCurrLine
    };

    if (tokens.isEmpty())
        return;

    switch (type)
    {
    case InfoDepth:
        eval->setDepth(tokens[0].toString().toInt());
        break;
    case InfoSelDepth:
        eval->setSelectiveDepth(tokens[0].toString().toInt());
        break;
    case InfoTime:
        eval->setTime(tokens[0].toString().toInt());
        break;
    case InfoNodes:
        eval->setNodeCount(tokens[0].toString().toULongLong());
        break;
    case InfoMultiPv:
        eval->setPvNumber(tokens[0].toString().toInt());
        break;
    case InfoPv:
        eval->setPv(m_useDirectPv ?  directPv(tokens) : sanPv(tokens));
        break;
    case InfoScore:
    {
        int score = 0;
        int i = 1;
        if (tokens[i - 1] == "cp")
            score = tokens[i].toString().toInt();
        else if (tokens[i - 1] == "mate")
        {
            score = tokens[i].toString().toInt();
            if (score > 0)
                score = eval->MATE_SCORE + 1 - score * 2;
            else if (score < 0)
                score = -eval->MATE_SCORE - score * 2;
        }
        else if (tokens[i - 1] == "lowerbound"
                 || tokens[i - 1] == "upperbound")
            return;
        else {
            score = tokens[i-1].toString().toInt();
        }
        if (redEvalPov() && side() == Chess::Side::Black)
            score = -score;
        eval->setScore(score);
    }
        break;
    case InfoNps:
        eval->setNps(tokens[0].toString().toULongLong());
        break;
    case InfoTbHits:
        eval->setTbHits(tokens[0].toString().toULongLong());
        break;
    case InfoHashFull:
        eval->setHashUsage(tokens[0].toString().toInt());
        break;
    default:
        break;
    }
}

void UcciEngine::parseInfo(const QStringRef& line)
{
    static const QString types[] =
    {
        "depth",
        "seldepth",
        "time",
        "nodes",
        "score",
        "pv",
        "multipv",
        "currmove",
        "currmovenumber",
        "hashfull",
        "nps",
        "tbhits",
        "cpuload",
        "string",
        "refutation",
        "currline"
    };

    int type = -1;
    QStringRef token(nextToken(line));
    QVarLengthArray<QStringRef> tokens;
    MoveEvaluation eval;

    // The "string" info is not supported and it can't be parsed
    // like other info lines.
    if (token == "string")
        return;

    while (!token.isNull())
    {
        token = parseUcciTokens(token, types, 16, tokens, type);
        parseInfo(tokens, type, &eval);
    }
    if (eval.isEmpty())
        return;

    if (!m_ponderMove.isNull())
        eval.setPonderMove(m_ponderMoveSan);
    if (m_movesPondered)
        eval.setPonderhitRate((m_ponderHits * 1000) / m_movesPondered);

    // Only the primary PV can be considered the current eval
    if (eval.pvNumber() <= 1)
    {
        m_eval.merge(eval);
        if (eval.depth() && eval.depth() != m_currentEval.depth())
            m_currentEval.clear();
        m_currentEval.merge(eval);

        emit thinking(m_currentEval);
    }
    else
        emit thinking(eval);
}

EngineOption* UcciEngine::parseOption(const QStringRef& line)
{
    enum Keyword
    {
        OptionName,
        OptionType,
        OptionDefault,
        OptionMin,
        OptionMax,
        OptionVar
    };
    static const QString types[] =
    {
        "option",
        "type",
        "default",
        "min",
        "max",
        "var"
    };

    QString name;
    QString type;
    QString value;
    QStringList choices;
    int min = 0;
    int max = 0;

    int keyword = -1;
    QStringRef token(line);
    QVarLengthArray<QStringRef> tokens;

    while (!token.isNull())
    {
        token = parseUcciTokens(token, types, 6, tokens, keyword);
        if (tokens.isEmpty() || keyword == -1)
            continue;

        QString str(joinTokens(tokens).toString());

        switch (keyword)
        {
        case OptionName:
            name = str;
            break;
        case OptionType:
            type = str;
            break;
        case OptionDefault:
            value = str;
            break;
        case OptionMin:
            min = str.toInt();
            break;
        case OptionMax:
            max = str.toInt();
            break;
        case OptionVar:
            choices << str;
            break;
        }
    }
    if (name.isEmpty())
        return nullptr;

    if (type == "button")
        return new EngineButtonOption(name);
    else if (type == "check")
    {
        if (value == "true")
            return new EngineCheckOption(name, true, true);
        else
            return new EngineCheckOption(name, false, false);
    }
    else if (type == "combo")
        return new EngineComboOption(name, value, value, choices);
    else if (type == "spin")
        return new EngineSpinOption(name, value.toInt(), value.toInt(), min, max);
    else if (type == "string")
        return new EngineTextOption(name, value, value);
    else if (type == "filename")
        return new EngineTextOption(name, value, value, QString(),
                                    EngineTextOption::FileDialog);

    return nullptr;
}

void UcciEngine::parseLine(const QString& line)
{
    const QStringRef command(firstToken(line));

    if (command == "info")
    {
        if (m_ignoreThinking)
            return;
        parseInfo(command);
    }
    else if (command == "bestmove")
    {
        bool wasPondering = isPondering();
        m_ponderState = NotPondering;
        if (m_ignoreThinking)
        {
            m_ignoreThinking = false;
            if (!m_bmBuffer.isEmpty())
            {
                const auto buf = m_bmBuffer;
                for (const auto& l : buf)
                    write(l, Unbuffered);
                m_bmBuffer.clear();
            }
            else
                pong();
            return;
        }
        else if (wasPondering)
        {
            qWarning("Premature bestmove while pondering from %s",
                     qUtf8Printable(name()));
            m_ponderMove = Chess::Move();
            m_ponderMoveSan.clear();
            m_moveStrings.truncate(m_moveStrings.lastIndexOf(' '));
            pong();
            return;
        }

        if (state() != Thinking)
        {
            if (state() == FiniGuardngGame)
                pong();
            else
                qWarning() << "Unexpected move from" << name();
            return;
        }

        QStringRef token(nextToken(command));
        QString moveString(token.toString());
        m_moveStrings += " " + moveString;
        Chess::Move move = board()->moveFromString(moveString);
        if (move.isNull())
        {
            forfeit(Chess::Result::IllegalMove, moveString);
            return;
        }

        if (m_canPonder && (token = nextToken(token)) == "ponder")
        {
            board()->makeMove(move);
            setPonderMove(nextToken(token).toString());
            board()->undoMove();
        }

        emitMove(move);
    }
    else if (command == "readyok")
    {
        if (m_rePing)
        {
            m_rePing = false;
            pong(false);
            ping();
        }
        else
            pong();
    }
    else if (command == "ucciok")
    {
        if (state() == Starting)
        {
            onProtocolStart();
            ping();
        }
    }
    else if (command == "id")
    {
        QStringRef tag(nextToken(command));
        if (tag == "name" && name() == "UcciEngine")
            setName(nextToken(tag, true).toString());
    }
    else if (command == "registration")
    {
        if (nextToken(command) == "error")
        {
            qWarning("Failed to register UCCI engine %s",
                     qUtf8Printable(name()));
            write("register later");
        }
    }
    else if (command == "option")
    {
        EngineOption* option = parseOption(command);
        QString variant;

        if (option == nullptr || !option->isValid())
            qWarning("Invalid UCCI option from %s: %s",
                     qUtf8Printable(name()), qUtf8Printable(line));
        else if (!(variant = variantFromUcci(option->name())).isEmpty())
            addVariant(variant);
        else if (option->name() == "UCCI_Variant")
            addVariantsFromOption(option);
        else if (option->name() == "UCCI_Opponent")
            m_sendOpponentsName = true;
        else if (option->name() == "ponder")
            m_canPonder = true;
        else if (option->name() == "usemillisec")
            m_useMillisec = option->value().toBool();
        else if (option->name().startsWith("UCCI_") &&
                 option->name() != "UCCI_LimitStrength" &&
                 option->name() != "UCCI_Elo")
        {
            // TODO: Deal with UCCI features
        }
        else
        {
            addOption(option);
            return;
        }

        delete option;
    }
}

void UcciEngine::addVariantsFromOption(const EngineOption* option)
{
    const auto combo = dynamic_cast<const EngineComboOption*>(option);
    if (!combo)
    {
        qWarning("Option %s from %s is not a combo option",
                 qUtf8Printable(option->name()),
                 qUtf8Printable(name()));
        return;
    }

    const auto choices = combo->choices();
    for (const auto& choice : choices)
    {
        QString variant = variantFromUcci(choice, false);
        if (!variant.isEmpty())
            addVariant(variant);
    }
    m_comboVariants = choices;
}

void UcciEngine::setVariant(const QString& variant)
{
    QString uciVariant(variantToUcci(variant));
    if (uciVariant != m_variantOption && !m_variantOption.isEmpty())
        sendOption(m_variantOption, false);

    if (m_comboVariants.contains(variant))
    {
        m_variantOption.clear();
        sendOption("UCI_Variant", variantToUcci(variant, false));
    }
    else
    {
        m_variantOption = uciVariant;
        if (!uciVariant.isEmpty())
            sendOption(uciVariant, true);
    }
}

void UcciEngine::setPonderMove(const QString& moveString)
{
    // Stockfish sends "(none)" and Komodo sends "0000"
    // when it responds to "ponderhit".
    if (moveString == "(none)" || moveString == "0000")
    {
        m_ponderMove = Chess::Move();
        m_ponderMoveSan.clear();
        return;
    }

    Chess::Board* board = this->board();

    m_ponderMove = board->moveFromString(moveString);
    if (m_ponderMove.isNull())
    {
        m_ponderMoveSan.clear();
        qWarning("Illegal ponder move from %s: %s",
                 qUtf8Printable(name()),
                 qUtf8Printable(moveString));
    }
    else
    {
        // Ignore game ending ponder moves
        board->makeMove(m_ponderMove);
        if (!board->result().isNone())
            m_ponderMove = Chess::Move();
        board->undoMove();

        if (!m_ponderMove.isNull())
            m_ponderMoveSan = board->moveString(m_ponderMove, Chess::Board::Standard);
    }
}

QString UcciEngine::directPv(const QVarLengthArray<QStringRef>& tokens)
{
    QString pv;
    for( auto token : tokens)
    {
        pv += " ";
        pv += token.toString();
    }
    return pv;
}

QString UcciEngine::sanPv(const QVarLengthArray<QStringRef>& tokens)
{
    Chess::Board* board = this->board();
    QString pv;
    int movesMade = 0;

    if (pondering() && !m_ponderMove.isNull())
    {
        board->makeMove(m_ponderMove);
        movesMade++;
    }

    for (auto token : tokens)
    {
        auto move = board->moveFromString(token.toString());
        if (move.isNull())
        {
            QString tokenString(token.toString());
            qWarning("Illegal PV move %s from %s (%d)",
                     qUtf8Printable(tokenString),
                     qUtf8Printable(name()),
                     id());
            qWarning("PV: %s %s",
                     qUtf8Printable(pv),
                     qUtf8Printable(tokenString));
            break;
        }
        if (!pv.isEmpty())
            pv += " ";
        pv += board->moveString(move, Chess::Board::Standard);
        board->makeMove(move);
        movesMade++;
    }

    for (int i = 0; i < movesMade; i++)
        board->undoMove();

    return pv;
}

void UcciEngine::sendOption(const QString& name, const QVariant& value)
{
    if (!value.isNull())
        write(QString("setoption %1 %2").arg(name, value.toString()));
    else
        write(QString("setoption %1").arg(name));
}
