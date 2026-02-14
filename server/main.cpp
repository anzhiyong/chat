#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QHostAddress>

#include "ChatServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("QtChatServer");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple Qt TCP chat server");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption hostOption(QStringList() << "H" << "host", "Host to bind", "host", "0.0.0.0");
    QCommandLineOption portOption(QStringList() << "p" << "port", "Port to bind", "port", "9000");
    parser.addOption(hostOption);
    parser.addOption(portOption);
    parser.process(app);

    bool ok = false;
    const int portInt = parser.value(portOption).toInt(&ok);
    if (!ok || portInt <= 0 || portInt > 65535) {
        qCritical("Invalid port. Use 1-65535.");
        return 1;
    }

    const QHostAddress address(parser.value(hostOption));
    if (address.isNull()) {
        qCritical("Invalid host address.");
        return 1;
    }

    ChatServer server;
    if (!server.start(address, static_cast<quint16>(portInt))) {
        return 1;
    }

    return app.exec();
}
