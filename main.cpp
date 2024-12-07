#include <QCoreApplication>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonObject>

QString decode_config(QString link);
inline QTextStream& qStdout();

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("amnezia_link_to_native_awg");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Amnezia link to native AWG config");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("link", "Input link.");

    parser.process(app);
    const QStringList links = parser.positionalArguments();
    if(links.size() != 1) qFatal("You must pass exactly one link.");

    QString link = links[0];
    QString configStr= decode_config(link);
    QJsonObject configObj =
        QJsonDocument::fromJson(configStr.toUtf8()).object();

    bool isAwgConf = configObj["defaultContainer"].toString() == "amnezia-awg";
    if(!isAwgConf) qFatal("The link does not have an AWG configuration.");

    QString lastConfigStr =
        configObj["containers"][0]["awg"]["last_config"].toString();
    QJsonObject lastConfigObj =
        QJsonDocument::fromJson(lastConfigStr.toUtf8()).object();

    QString printStr = lastConfigObj["config"].toString();
    printStr.replace("$PRIMARY_DNS",   configObj["dns1"].toString());
    printStr.replace("$SECONDARY_DNS", configObj["dns2"].toString());

    qStdout() << printStr << Qt::endl;

    app.exit(0);
}

QString decode_config(QString link) {
    link.replace("vpn://", "");
    auto ba = QByteArray::fromBase64(link.toUtf8(),
        QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    QByteArray ba_uncompressed = qUncompress(ba);
    if (!ba_uncompressed.isEmpty()) ba = ba_uncompressed;
    QString config = ba;
    return config;
}

inline QTextStream& qStdout() {
    static QTextStream r{stdout};
    return r;
}
