/*
 * Copyright 2012, Tim Baker <treectrl@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "preferences.h"
#include "tilemetainfomgr.h"

#include <QFileDialog>
#include <QStringList>
#include <QDir>
#include <QStyleFactory>

PreferencesDialog::PreferencesDialog(WorldDocument *worldDoc, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
    , mWorldDoc(worldDoc)
{
    ui->setupUi(this);

    Preferences *prefs = Preferences::instance();

    connect(ui->themeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PreferencesDialog::themeChanged);
    connect(ui->fusionStyleMode, &QAbstractButton::toggled, this, &PreferencesDialog::fusionStyleModeToggled);
    connect(ui->vistaStyleMode, &QAbstractButton::toggled, this, &PreferencesDialog::vistaStyleModeToggled);
    connect(ui->windowsStyleMode, &QAbstractButton::toggled, this, &PreferencesDialog::windowsStyleModeToggled);

    mTilesDirectory = prefs->tilesDirectory();
    ui->tilesDirectory->setText(QDir::toNativeSeparators(mTilesDirectory));
    connect(ui->browseTilesDirectory, &QAbstractButton::clicked,
            this, &PreferencesDialog::browseTilesDirectory);

    QString configPath = prefs->configPath();
    ui->configDirectory->setText(QDir::toNativeSeparators(configPath));
    mTileZedPath = prefs->tileZedPath();
    ui->tileZedPath->setText(QDir::toNativeSeparators(mTileZedPath));

    mGridColor = prefs->gridColor();
    ui->gridColor->setColor(mGridColor);
    connect(ui->gridColor, &Tiled::Internal::ColorButton::colorChanged,
            this, &PreferencesDialog::gridColorChanged);

    ui->openGL->setChecked(prefs->useOpenGL());
    ui->thumbnails->setChecked(prefs->loadAllWorldThumbnails());
    ui->showAdjacent->setChecked(prefs->showAdjacentMaps());
    ui->zombieSpawnImageOpacity->setValue(int(prefs->zombieSpawnImageOpacity() * 100));

    ui->themeCombo->setCurrentText(prefs->theme());
    if (prefs->styleMode().compare(QLatin1String("fusion"), Qt::CaseInsensitive) == 0) {
        ui->fusionStyleMode->setChecked(true);
    } else if (prefs->styleMode().compare(QLatin1String("windowsvista"), Qt::CaseInsensitive) == 0) {
        ui->vistaStyleMode->setChecked(true);
    } else {
        ui->windowsStyleMode->setChecked(true);
    }
    updateStyleModeUi();

    // Unofficial Fork - begin
    connect(ui->browseTileZedPath, &QAbstractButton::clicked,
            this, &PreferencesDialog::browseTileZedPath);

    ui->LoadLastActiv->setChecked(prefs->LoadLastActivProject());
    ui->hsThresholdHP->setValue(int(prefs->hsThresholdHP()));
    ui->lblThresholdHP->setText(QString::number(int(ui->hsThresholdHP->value())));
    ui->hsThresholdHT->setValue(int(prefs->hsThresholdHT()));
    ui->lblThresholdHT->setText(QString::number(int(ui->hsThresholdHT->value())));
    ui->hsThresholdR->setValue(int(prefs->hsThresholdR()));
    ui->lblThresholdR->setText(QString::number(int(ui->hsThresholdR->value())));
    ui->hsSizeHP->setValue(int(prefs->hsSizeHP()));
    ui->lblSizeHP->setText(QString::number(int(ui->hsSizeHP->value())));
    ui->hsSizeHT->setValue(int(prefs->hsSizeHT()));
    ui->lblSizeHT->setText(QString::number(int(ui->hsSizeHT->value())));
    ui->hsSizeR->setValue(int(prefs->hsSizeR()));
    ui->lblSizeR->setText(QString::number(int(ui->hsSizeR->value())));

    ui->gridOpacity->setValue(int(prefs->GridOpacity()));
    ui->gridWidth->setValue(int(prefs->GridWidth()));
    ui->thumbWidth->setValue(int(prefs->ThumbWidth()));

    connect(ui->gridOpacity, qOverload<int>(&QSpinBox::valueChanged), Preferences::instance(), &Preferences::setGridOpacity);
    connect(ui->gridWidth, qOverload<int>(&QSpinBox::valueChanged), Preferences::instance(), &Preferences::setGridWidth);
    connect(ui->thumbWidthDefault, &QPushButton::clicked, this, &PreferencesDialog::resetThumbWidth);

    connect(ui->hsSizeR, &QSlider::valueChanged, this, [this](int value) {
        ui->lblSizeR->setText(QString::number(value));
    });

    connect(ui->hsSizeHP, &QSlider::valueChanged, this, [this](int value) {
        ui->lblSizeHP->setText(QString::number(value));
    });
    connect(ui->hsThresholdHP, &QSlider::valueChanged, this, [this](int value) {
        ui->lblThresholdHP->setText(QString::number(value));
    });
    connect(ui->hsThresholdHT, &QSlider::valueChanged, this, [this](int value) {
        ui->lblThresholdHT->setText(QString::number(value));
    });
    connect(ui->hsThresholdR, &QSlider::valueChanged, this, [this](int value) {
        ui->lblThresholdR->setText(QString::number(value));
    });
    connect(ui->hsSizeHT, &QSlider::valueChanged, this, [this](int value) {
        ui->lblSizeHT->setText(QString::number(value));
    });
    connect(ui->hsSizeR, &QSlider::valueChanged, this, [this](int value) {
        ui->lblSizeR->setText(QString::number(value));
    });

    connect(ui->thumbWidthDefault, &QPushButton::clicked, this, &PreferencesDialog::resetThumbWidth);
        connect(ui->gridWidthDefault, &QPushButton::clicked, this, [this]() {
            ui->gridWidth->setValue(1); // Valeur par défaut pour GridWidth
        });
        connect(ui->gridOpacityDefault, &QPushButton::clicked, this, [this]() {
            ui->gridOpacity->setValue(255); // Valeur par défaut pour GridOpacity
        });
        connect(ui->hsThresholdHPDefault, &QPushButton::clicked, this, [this]() {
            ui->hsThresholdHP->setValue(1000); // Valeur par défaut pour ThresholdHP
        });
        connect(ui->hsSizeHPDefault, &QPushButton::clicked, this, [this]() {
            ui->hsSizeHP->setValue(40); // Valeur par défaut pour SizeHP
        });
        connect(ui->hsThresholdHTDefault, &QPushButton::clicked, this, [this]() {
            ui->hsThresholdHT->setValue(1000); // Valeur par défaut pour ThresholdHP
        });
        connect(ui->hsSizeHTDefault, &QPushButton::clicked, this, [this]() {
            ui->hsSizeHT->setValue(40); // Valeur par défaut pour SizeHP
        });
        connect(ui->hsThresholdRDefault, &QPushButton::clicked, this, [this]() {
            ui->hsThresholdR->setValue(1000); // Valeur par défaut pour ThresholdHP
        });
        connect(ui->hsSizeRDefault, &QPushButton::clicked, this, [this]() {
            ui->hsSizeR->setValue(40); // Valeur par défaut pour SizeHP
        });
    // Unofficial Fork - end
}

void PreferencesDialog::resetThumbWidth() {
    ui->thumbWidth->setValue(512); // Définit la valeur par défaut
}

void PreferencesDialog::browseTileZedPath()
{
    QString f = QFileDialog::getExistingDirectory(this, tr("TileZed Path"),
                                                  ui->tileZedPath->text());
    if (!f.isEmpty()) {
        mTileZedPath = f;
        ui->tileZedPath->setText(QDir::toNativeSeparators(f));
    }
}

void PreferencesDialog::browseTilesDirectory()
{
    QString f = QFileDialog::getExistingDirectory(this, tr("Tiles Directory"),
                                                  ui->tilesDirectory->text());
    if (!f.isEmpty()) {
        mTilesDirectory = f;
        ui->tilesDirectory->setText(QDir::toNativeSeparators(f));
    }
}

void PreferencesDialog::themeChanged(int index)
{
    Q_UNUSED(index)
    QString text = ui->themeCombo->currentText();
    Preferences::instance()->setTheme(text);
}


void PreferencesDialog::fusionStyleModeToggled(bool checked)
{
    if (!checked)
        return;

    Preferences::instance()->setStyleMode(QLatin1String("fusion"));
    updateStyleModeUi();
}

void PreferencesDialog::vistaStyleModeToggled(bool checked)
{
    if (!checked)
        return;

    Preferences::instance()->setStyleMode(QLatin1String("windowsvista"));
    updateStyleModeUi();
}

void PreferencesDialog::windowsStyleModeToggled(bool checked)
{
    if (!checked)
        return;

    Preferences::instance()->setStyleMode(QLatin1String("windows"));
    updateStyleModeUi();
}

void PreferencesDialog::updateStyleModeUi()
{
    const QStringList styleKeys = QStyleFactory::keys();
    auto hasStyle = [&styleKeys](const QString &name) {
        for (const QString &key : styleKeys) {
            if (key.compare(name, Qt::CaseInsensitive) == 0)
                return true;
        }
        return false;
    };

    // these really would be better overall as some combined thing or a switch, but its only 3 so whatever
    const bool hasWindows = hasStyle(QLatin1String("windows"));
    const bool hasWindowsVista = hasStyle(QLatin1String("windowsvista"));
    const bool hasFusion = hasStyle(QLatin1String("fusion"));

    ui->fusionStyleMode->setEnabled(hasFusion);
    ui->vistaStyleMode->setEnabled(hasWindowsVista);
    ui->windowsStyleMode->setEnabled(hasWindows);

    if (!ui->fusionStyleMode->isChecked() && !ui->vistaStyleMode->isChecked() && !ui->windowsStyleMode->isChecked()) {
        if (hasFusion)
            ui->fusionStyleMode->setChecked(true);
        else if (hasWindowsVista)
            ui->vistaStyleMode->setChecked(true);
        else if (hasWindows)
            ui->windowsStyleMode->setChecked(true);
    }
}

void PreferencesDialog::gridColorChanged(const QColor &gridColor)
{
    mGridColor = gridColor;
}

void PreferencesDialog::accept()
{
    QDialog::accept();

    Preferences *prefs = Preferences::instance();
    Tiled::TileMetaInfoMgr::instance()->changeTilesDirectory(mTilesDirectory);
    prefs->setUseOpenGL(ui->openGL->isChecked());
    prefs->setLoadAllWorldThumbnails(ui->thumbnails->isChecked());
    prefs->setGridColor(mGridColor);
    prefs->setShowAdjacentMaps(ui->showAdjacent->isChecked());
    prefs->setZombieSpawnImageOpacity(ui->zombieSpawnImageOpacity->value() / 100.0);

    prefs->setHsThresholdHP(ui->hsThresholdHP->value());
    prefs->setHsSizeHP(ui->hsSizeHP->value());
    prefs->setHsThresholdHT(ui->hsThresholdHT->value());
    prefs->setHsSizeHT(ui->hsSizeHT->value());
    prefs->setHsThresholdR(ui->hsThresholdR->value());
    prefs->setHsSizeR(ui->hsSizeR->value());

    prefs->setGridOpacity(ui->gridOpacity->value());
    prefs->setGridWidth(ui->gridWidth->value());
    prefs->setThumbWidth(ui->thumbWidth->value());

    prefs->setLoadLastActivProject(ui->LoadLastActiv->isChecked());
    prefs->setTileZedPath(mTileZedPath);
}
