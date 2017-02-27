#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "EmulatorCore.h"
#include "Keyboard.h"

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(EmulatorCore &emu, QWidget *parent = 0);

    ~SettingsDialog();

private slots:
    void on_keyMapTable_itemSelectionChanged();

    void on_changeKeyBtn_clicked();

    void on_buttonBox_accepted();

    void on_newColorButton_clicked();

    void on_speakerEnable_toggled(bool checked);

private:
    QString key_tostring(QKeyEvent key);

    void keyPressEvent(QKeyEvent *key) override;

private:
    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::SettingsDialog *_ui;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;

    /**
     * The modified key mappings before they get written to the Keyboard module.
     */
    key_mappings _key_map;

    /**
     * The video module's text color.
     */
    QColor _text_color;

    /**
     * True if speaker is muted, false otherwise.
     */
    bool _speaker_mute;

    /**
     * QKeyEvent for the selected key mapping.
     */
    QKeyEvent _selected_keyevent;

    /**
     * Scancode for the selected key mapping.
     */
    Scancode _selected_scancode;

    /**
     * Row number for the selected key mapping in the table widget.
     */
    int _selected_row;

    /**
     * True if waiting for a key press, false otherwise.
     */
    bool _waiting_for_press;
};

#endif // SETTINGSDIALOG_H
