#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QShortcut>
#include <QCheckBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QComboBox>
#include <QDebug>
#include <nlohmann/json.hpp>
#include <fstream>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>

using json = nlohmann::json;

int main(int argc, char* argv[])
{

    QApplication app(argc, argv);

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString configFile = configDir + "/config.json";

    QDir dir(configDir);
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            qDebug() << "Cartella creata:" << configDir;
        }
        else {
            qDebug() << QObject::tr("ERRORE: impossibile creare") << configDir;
        }
    }

    QMainWindow window;
    window.setWindowTitle(QObject::tr("Empireo Media Engine"));
    window.resize(800, 600);
    window.setWindowIcon(QIcon("resources/velo.ico"));

    QWidget* central = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    window.setCentralWidget(central);

    QWidget* settingswindow = new QWidget(&window, Qt::Window);
    settingswindow->setWindowTitle(QObject::tr("Impostazioni"));
    settingswindow->resize(400, 300);

    QObject::connect(&window, &QWidget::destroyed, settingswindow, &QWidget::close);

    // ─── MENU BAR ────────────────────────────────────────────────
    QMenuBar* menuBar = window.menuBar();
    QMenu* infoMenu = menuBar->addMenu(QObject::tr("Info"));
    QMenu* preferenze = menuBar->addMenu(QObject::tr("Preferenze"));

    QAction* aboutAction = new QAction(QObject::tr("Informazioni"), &window);
    QAction* reportAction = new QAction(QObject::tr("Segnalazione problemi"), &window);
    QAction* commandsAction = new QAction(QObject::tr("Comandi, shortcut, schermo intero..."), &window);
    QAction* settingsAction = new QAction(QObject::tr("Impostazioni"), &window);

    infoMenu->addAction(aboutAction);
    infoMenu->addAction(reportAction);
    infoMenu->addAction(commandsAction);
    preferenze->addAction(settingsAction);

    // Connessioni menu
    QObject::connect(aboutAction, &QAction::triggered, [&] {
        QMessageBox::information(&window, QObject::tr("Informazioni"),
            QObject::tr("NOME PROGRAMMA: Empireo Media Engine\n\n"
                "FONDATORE E PROPRIETARIO: Sgarbelo_G\n\n"
                "VERSIONE: 8.0\n"
                "Tutti i diritti di autore sono riservati a: Sgarbelo_G\n\n\n"
                "𝑬𝒎𝒑𝒊𝒓𝒆𝒐 - 𝒇𝒆𝒆𝒍 𝒕𝒉𝒆 𝒔𝒌𝒚"));
        });

    QObject::connect(reportAction, &QAction::triggered, [&] {
        QMessageBox::information(&window, QObject::tr("Segnala un problema"),
            QObject::tr("Per segnalare qualsiasi problema riscontrato\n"
                "aprire ticket sul canale discord\n"
                "Developing with Sgarbelo_G\n"
                "https://discord.gg/aBjasU73up\n"
                "o contattami per email:\n"
                "Sgarbelo_G@protonmail.com"));
        });

    QObject::connect(commandsAction, &QAction::triggered, [&] {
        QMessageBox::information(&window, QObject::tr("Comandi e Scorciatoie"),
            QObject::tr("Per uscire dallo schermo intero\n"
                "premi il tasto ESC oppure il tasto F\n\n"
                "Formato durata file: minuti:secondi"));
        });

    QObject::connect(settingsAction, &QAction::triggered, [&] {
        settingswindow->show();
        settingswindow->raise();
        settingswindow->activateWindow();
        });

    // Player
    QMediaPlayer* player = new QMediaPlayer(&window);
    QAudioOutput* audio = new QAudioOutput(&window);
    player->setAudioOutput(audio);

    QVideoWidget* videoWidget = new QVideoWidget;
    player->setVideoOutput(videoWidget);
    videoWidget->setMinimumHeight(300);

    // Widget di controllo
    QSlider* volumeSlider = new QSlider(Qt::Vertical);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    audio->setVolume(0.8);

    QLabel* volumeLabel = new QLabel(QObject::tr("Volume: 80"));
    QLabel* fileNameLabel = new QLabel(QObject::tr("Nessun file aperto"));
    QSlider* positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    QPushButton* openBtn = new QPushButton(QObject::tr(" ^ Apri file ^ "));
    QPushButton* playBtn = new QPushButton(" ▶️ ");
    QPushButton* pauseBtn = new QPushButton(" ⏸️ ");
    QPushButton* playUnified = new QPushButton("▶️");
    QPushButton* pauseUnified = new QPushButton("⏸️");
    QPushButton* fullscreenBtn = new QPushButton("⛶");
    QPushButton* resetVolumeBtn = new QPushButton(QObject::tr("Reset volume"));

    playUnified->hide();
    pauseUnified->hide();

    QCheckBox* unifiedPlayPause = new QCheckBox(QObject::tr("Tasto riproduci e pausa uniti"));
    QCheckBox* muteCheck = new QCheckBox("🔊");
    QCheckBox* loopCheck = new QCheckBox(QObject::tr("Loop 🔁"));

    QComboBox* speedBox = new QComboBox;
    speedBox->addItem("0.1x", 0.1); speedBox->addItem("0.2x", 0.2);
    speedBox->addItem("0.3x", 0.3); speedBox->addItem("0.4x", 0.4);
    speedBox->addItem("0.5x", 0.5); speedBox->addItem("0.6x", 0.6);
    speedBox->addItem("0.7x", 0.7); speedBox->addItem("0.75x", 0.75);
    speedBox->addItem("0.8x", 0.8); speedBox->addItem("0.9x", 0.9);
    speedBox->addItem(QObject::tr("1.0x normale"), 1.0);
    speedBox->addItem("1.1x", 1.1); speedBox->addItem("1.2x", 1.2);
    speedBox->addItem("1.25x", 1.25); speedBox->addItem("1.3x", 1.3);
    speedBox->addItem("1.4x", 1.4); speedBox->addItem("1.5x", 1.5);
    speedBox->addItem("1.6x", 1.6); speedBox->addItem("1.7x", 1.7);
    speedBox->addItem("1.8x", 1.8); speedBox->addItem("1.9x", 1.9);
    speedBox->addItem("2.0x", 2.0); speedBox->addItem("3.0x", 3.0);
    speedBox->addItem("4.0x", 4.0); speedBox->addItem("5.0x", 5.0);
    speedBox->addItem("6.0x", 6.0); speedBox->addItem("7.0x", 7.0);
    speedBox->addItem("8.0x", 8.0); speedBox->addItem("9.0x", 9.0);
    speedBox->addItem("10.0x", 10.0);
    speedBox->setStyleSheet("font-size: 20px; font-weight: bold;");

    openBtn->setMinimumHeight(40);
    openBtn->setStyleSheet("font-size: 18px;");
    playBtn->setStyleSheet("font-size: 32px; font-weight: bold;");
    pauseBtn->setStyleSheet("font-size: 32px; font-weight: bold;");
    playUnified->setStyleSheet("font-size: 32px; font-weight: bold;");
    pauseUnified->setStyleSheet("font-size: 32px; font-weight: bold;");
    fullscreenBtn->setStyleSheet("font-size: 32px; font-weight: bold;");
    muteCheck->setStyleSheet("font-size: 28px; font-weight: bold;");
    loopCheck->setStyleSheet("font-size: 20px; font-weight: bold;");
    fileNameLabel->setStyleSheet("font-size: 24px;");

    QLabel* durationLabel = new QLabel(QObject::tr("Durata: 0:00"));

    // Layout impostazioni
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingswindow);
    settingsLayout->addWidget(unifiedPlayPause);
    settingsLayout->addStretch();

    // Layout principale
    QHBoxLayout* videoRow = new QHBoxLayout;
    videoRow->addWidget(volumeSlider);

    QVBoxLayout* videoContainer = new QVBoxLayout;
    videoContainer->addWidget(videoWidget);
    videoRow->addLayout(videoContainer, 1);

    QHBoxLayout* controls = new QHBoxLayout;
    controls->addWidget(playBtn);
    controls->addWidget(pauseBtn);
    controls->addWidget(playUnified);
    controls->addWidget(pauseUnified);
    controls->addWidget(fullscreenBtn);
    controls->addWidget(speedBox);
    controls->addStretch();
    controls->addWidget(loopCheck);

    mainLayout->addLayout(videoRow);
    mainLayout->addWidget(positionSlider);
    mainLayout->addWidget(volumeLabel);
    mainLayout->addWidget(durationLabel);
    mainLayout->addWidget(muteCheck, 0, Qt::AlignLeft);
    mainLayout->addWidget(resetVolumeBtn, 0, Qt::AlignLeft);
    mainLayout->addWidget(fileNameLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(controls);
    mainLayout->addWidget(openBtn);

    // ─── Connessioni ─────────────────────────────────────────────

    QObject::connect(loopCheck, &QCheckBox::stateChanged, [&](int state) {
        player->setLoops(state == Qt::Checked ? QMediaPlayer::Infinite : 1);
        });

    // Shortcuts
    QShortcut* space = new QShortcut(QKeySequence(Qt::Key_Space), &window, nullptr, nullptr, Qt::ApplicationShortcut);
    QObject::connect(space, &QShortcut::activated, [&] {
        if (player->playbackState() == QMediaPlayer::PlayingState)
            player->pause();
        else
            player->play();
        });

    bool isFullScreen = false;
    QObject::connect(fullscreenBtn, &QPushButton::clicked, [&] {
        isFullScreen = !isFullScreen;
        videoWidget->setFullScreen(isFullScreen);
        });
    QShortcut* esc = new QShortcut(QKeySequence(Qt::Key_Escape), &window, nullptr, nullptr, Qt::ApplicationShortcut);
    QShortcut* f = new QShortcut(QKeySequence(Qt::Key_F), &window, nullptr, nullptr, Qt::ApplicationShortcut);
    QShortcut* leftarrow = new QShortcut(QKeySequence(Qt::Key_Left), &window, nullptr, nullptr, Qt::ApplicationShortcut);
    QShortcut* rightarrow = new QShortcut(QKeySequence(Qt::Key_Right), &window, nullptr, nullptr, Qt::ApplicationShortcut);
    auto toggleFullscreen = [&] {
        isFullScreen = !isFullScreen;
        videoWidget->setFullScreen(isFullScreen);
        };
    QObject::connect(esc, &QShortcut::activated, toggleFullscreen);
    QObject::connect(f, &QShortcut::activated, toggleFullscreen);


    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Left), &window), &QShortcut::activated, [&] {
        qint64 pos = player->position();
        player->setPosition(std::max<qint64>(0, pos - 5000));
        });

    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Right), &window), &QShortcut::activated, [&] {
        qint64 pos = player->position();
        player->setPosition(std::min<qint64>(player->duration(), pos + 5000));
        });


    int prevVolume = 80;
    QObject::connect(muteCheck, &QCheckBox::stateChanged, [&](int state) {
        if (state == Qt::Checked) {
            prevVolume = volumeSlider->value();
            volumeSlider->setValue(0);
            muteCheck->setText("🔇");
        }
        else {
            volumeSlider->setValue(prevVolume);
            muteCheck->setText("🔊");
        }
        });

    QObject::connect(volumeSlider, &QSlider::valueChanged, [&](int v) {
        audio->setVolume(v / 100.0);
        volumeLabel->setText(QObject::tr("Volume: %1").arg(v));
        });


    QObject::connect(unifiedPlayPause, &QCheckBox::stateChanged, [&](int state) {
        bool unified = (state == Qt::Checked);
        playBtn->setVisible(!unified);
        pauseBtn->setVisible(!unified);

        if (unified) {
            if (player->playbackState() == QMediaPlayer::PlayingState) {
                pauseUnified->show();
                playUnified->hide();
            }
            else {
                playUnified->show();
                pauseUnified->hide();
            }
        }
        else {
            playUnified->hide();
            pauseUnified->hide();
        }
        });


    QObject::connect(openBtn, &QPushButton::clicked, [&] {
        QString file = QFileDialog::getOpenFileName(&window,
            QObject::tr("Seleziona un file"), "",
            QObject::tr("Media (*.mp3 *.mp4 *.mkv *.avi *.mov *.webm *.ogg *.flac *.wav *.m4a *.mp2 *.opus *.m4v *.wmv *.flv *.gif *.webp *.png *.jpg *.jpeg);;"
                "Tutti i file (*)"));

        if (!file.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(file));
            player->play();
            fileNameLabel->setText(QFileInfo(file).fileName());
        }
        });

    // Caricamento da argomenti
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1) {
        QString path = args.at(1);
        player->setSource(QUrl::fromLocalFile(path));
        player->play();
        fileNameLabel->setText(QFileInfo(path).fileName());
    }

    // ─── Salvataggio / Caricamento Configurazione ─────────────────
    json config;
    std::ifstream in(configFile.toStdString());
    if (in.is_open()) {
        try {
            in >> config;
            if (config.contains("volume")) {
                int v = config["volume"].get<int>();
                volumeSlider->setValue(v);
                audio->setVolume(v / 100.0);
                volumeLabel->setText(QObject::tr("Volume: %1").arg(v));
            }
            if (config.contains("speed")) {
                double s = config["speed"].get<double>();
                player->setPlaybackRate(s);
                int idx = speedBox->findData(s);
                if (idx >= 0) speedBox->setCurrentIndex(idx);
            }
            if (config.contains("unifiedPlayPause")) {
                unifiedPlayPause->setChecked(config["unifiedPlayPause"].get<bool>());
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Errore lettura JSON:" << e.what();
        }
    }

    // Salvataggio alla chiusura
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&] {
        json save;
        save["volume"] = volumeSlider->value();
        save["speed"] = player->playbackRate();
        save["unifiedPlayPause"] = unifiedPlayPause->isChecked();

        std::ofstream out(configFile.toStdString());
        if (out.is_open()) {
            out << save.dump(4);
            qDebug() << "Configurazione salvata";
        }
        });

    window.show();
    return app.exec();
}