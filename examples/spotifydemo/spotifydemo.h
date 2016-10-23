#ifndef SPOTIFYDEMO_H
#define SPOTIFYDEMO_H

#include <QObject>

#include "o2spotify.h"

class SpotifyDemo : public QObject
{
    Q_OBJECT

public:
    explicit SpotifyDemo(QObject *parent = 0);

signals:
    void extraTokensReady(const QVariantMap &extraTokens);
    void linkingFailed();
    void linkingSucceeded();

public slots:
    void doOAuth(O2::GrantFlow grantFlowType);
    void validateToken();

private slots:
    void onLinkedChanged();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();
    void onFinished();

private:
    O2Spotify *o2Spotify_;
};

#endif // SPOTIFYDEMO_H
