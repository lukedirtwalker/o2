#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QMetaEnum>
#include <QDebug>

#include "spotifydemo.h"
#include "o0globals.h"
#include "o0settingsstore.h"

const char SPOTIFY_APP_KEY[] = ""; // TODO use your own app key
const char SPOTIFY_APP_SECRET[] = ""; // TODO use your own app secret

const char SPOTIFY_REQUEST_URL[] = "https://accounts.spotify.com/authorize";
const char SPOTIFY_DEBUG_TOKEN[] = "https://api.spotify.com/v1/me?access_token=%1";

#define QENUM_NAME(o,e,v) (o::staticMetaObject.enumerator(o::staticMetaObject.indexOfEnumerator(#e)).valueToKey((v)))
#define GRANTFLOW_STR(v) QString(QENUM_NAME(O2, GrantFlow, v))

SpotifyDemo::SpotifyDemo(QObject *parent) :
    QObject(parent) {
    o2Spotify_ = new O2Spotify(this);

    o2Spotify_->setClientId(SPOTIFY_APP_KEY);
    o2Spotify_->setClientSecret(SPOTIFY_APP_SECRET);
    o2Spotify_->setRequestUrl(SPOTIFY_REQUEST_URL);
    o2Spotify_->setLocalhostPolicy("http://127.0.0.1:%1");
    o2Spotify_->setScope(O2Spotify::Scope::allScopesList().join(","));

    // Create a store object for writing the received tokens
    O0SettingsStore *store = new O0SettingsStore(O2_ENCRYPTION_KEY);
    store->setGroupKey("spotify");
    o2Spotify_->setStore(store);

    connect(o2Spotify_, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(o2Spotify_, SIGNAL(linkingFailed()), this, SIGNAL(linkingFailed()));
    connect(o2Spotify_, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(o2Spotify_, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(o2Spotify_, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));
}

void SpotifyDemo::doOAuth(O2::GrantFlow grantFlowType) {
    qDebug() << "Starting OAuth 2 with grant flow type" << GRANTFLOW_STR(grantFlowType) << "...";
    o2Spotify_->setGrantFlow(grantFlowType);
    o2Spotify_->unlink();
    o2Spotify_->link();
}

void SpotifyDemo::validateToken() {
    if (!o2Spotify_->linked()) {
        qWarning() << "ERROR: Application is not linked!";
        emit linkingFailed();
        return;
    }

    QString accessToken = o2Spotify_->token();
    QString debugUrlStr = QString(SPOTIFY_DEBUG_TOKEN).arg(accessToken);
    QNetworkRequest request = QNetworkRequest(QUrl(debugUrlStr));
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkReply *reply = mgr->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    qDebug() << "Validating user token. Please wait...";
}

void SpotifyDemo::onOpenBrowser(const QUrl &url) {
    QDesktopServices::openUrl(url);
}

void SpotifyDemo::onCloseBrowser() {
    qDebug() << "Close browser?";
}

void SpotifyDemo::onLinkedChanged() {
    qDebug() << "Link changed!";
}


void SpotifyDemo::onLinkingSucceeded() {
    O2Spotify *o1t = qobject_cast<O2Spotify *>(sender());
    if (!o1t->linked()) {
        return;
    }
    QVariantMap extraTokens = o1t->extraTokens();
    if (!extraTokens.isEmpty()) {
        emit extraTokensReady(extraTokens);
        qDebug() << "Extra tokens in response:";
        foreach (QString key, extraTokens.keys()) {
            qDebug() << "\t" << key << ":" << (extraTokens.value(key).toString().left(3) + "...");
        }
    }
    emit linkingSucceeded();
}

void SpotifyDemo::onFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qWarning() << "NULL reply!";
        emit linkingFailed();
        return;
    }

    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Reply error:" << reply->error();
        qWarning() << "Reason:" << reply->errorString();
        emit linkingFailed();
        return;
    }

    QByteArray replyData = reply->readAll();
    qDebug() << replyData;
    bool valid = !replyData.contains("error");
    if (valid) {
        qDebug() << "Token is valid";
        emit linkingSucceeded();
    } else {
        qDebug() << "Token is invalid";
        emit linkingFailed();
    }
}
