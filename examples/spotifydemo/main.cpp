#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include <iostream>

#include "spotifydemo.h"

const char OPT_OAUTH_CODE[] = "-o";
const char OPT_VALIDATE_TOKEN[] = "-v";

const char USAGE[] = "\n"
                     "Usage: spotifydemo [OPTION]...\n"
                     "Get OAuth2 access tokens from Spotify's OAuth service\n"
                     "\nOptions:\n"
                     "  %1\t\tLink with Spotify OAuth2 service using Authorization Code\n"
                     "  %2\t\tValidate Access Token\n";

class Helper : public QObject {
    Q_OBJECT

public:
    Helper() : QObject(), spotifyDemo_(this), waitForMsg_(false), msg_(QString()) {}

public slots:
    void processArgs() {
        QStringList argList = qApp->arguments();
        QByteArray help = QString(USAGE).arg(OPT_OAUTH_CODE,
                                             OPT_VALIDATE_TOKEN).toLatin1();
        const char* helpText = help.constData();
        connect(&spotifyDemo_, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
        connect(&spotifyDemo_, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
        if (argList.contains(OPT_OAUTH_CODE)) {
            // Start OAuth
            spotifyDemo_.doOAuth(O2::GrantFlowAuthorizationCode);
        } else if (argList.contains(OPT_VALIDATE_TOKEN)) {
            spotifyDemo_.validateToken();
        } else {
            qDebug() << helpText;
            qApp->exit(1);
        }
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        if (waitForMsg_) {
            //postStatusUpdate(msg_);
        } else {
            qApp->quit();
        }
    }

private:
    SpotifyDemo spotifyDemo_;
    bool waitForMsg_;
    QString msg_;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
     std::cout << "Helo";
    Helper helper;
     qDebug() << "Send single shot";
    QTimer::singleShot(0, &helper, SLOT(processArgs()));
    return a.exec();
}

#include "main.moc"
