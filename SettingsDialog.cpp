#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QColorDialog>
#include <QMessageBox>

/**
 * Constructor.
 *
 * @param parent Parent widget for this dialog.
 */
SettingsDialog::SettingsDialog(EmulatorCore &emu, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::SettingsDialog),
    _emu(emu),
    _key_map(_emu.GetMappings()),
    _text_color(),
    _speaker_mute(false),
    _selected_keyevent({ QEvent::KeyPress, Qt::Key_unknown, Qt::NoModifier }),
    _selected_scancode({0, ""}),
    _selected_row(0),
    _waiting_for_press(false)
{
    _ui->setupUi(this);

    /**
     * Initialize the keyboard mappings table.
     */
    _ui->keyMapTable->setHorizontalHeaderLabels({"Apple Scancode",
                                                 "Apple Key",
                                                 "Key Mapping"});

    _ui->keyMapTable->setRowCount(_key_map.size());

    int row = 0;
    for(auto mapping : _key_map)
    {
        QString scancode_str = "0x" + tr("%1").arg(mapping.second.code, 2, 16, QChar('0'));
        _ui->keyMapTable->setItem(row, 0, new QTableWidgetItem(scancode_str));
        _ui->keyMapTable->setItem(row, 1, new QTableWidgetItem(mapping.second.text));
        _ui->keyMapTable->setItem(row, 2, new QTableWidgetItem(key_tostring(mapping.first)));

        _ui->keyMapTable->item(row, 0)->setData(1, row);

        row++;
    }

    _ui->keyMapTable->sortItems(0);

    /**
     * Initialize the video text color picker.
     */
    _text_color = _emu.GetVideoTextColor();
    _ui->redEdit->setText("0x" +
            tr("%1").arg(_text_color.red(), 2, 16, QChar('0')));
    _ui->greenEdit->setText("0x" +
            tr("%1").arg(_text_color.green(), 2, 16, QChar('0')));
    _ui->blueEdit->setText("0x" +
            tr("%1").arg(_text_color.blue(), 2, 16, QChar('0')));

    /**
     * Initialize speaker mute.
     */
    _speaker_mute = _emu.GetSpeakerMute();
    _ui->speakerEnable->setChecked(_speaker_mute);
    _ui->speakerDisable->setChecked(!_speaker_mute);
}

/**
 * Converts the given key event into a string.
 *
 * @param key The key event to convert.
 *
 * @return A string representing the key.
 */
QString SettingsDialog::key_tostring(QKeyEvent key)
{
    const Qt::KeyboardModifiers mods = key.modifiers();
    QString qt_key = (mods & Qt::ControlModifier) ? "CTRL+" : "";
    qt_key += (mods & Qt::ShiftModifier) ? "SHIFT+" : "";
    qt_key += (mods & Qt::AltModifier) ? "ALT+" : "";
    qt_key += QKeySequence(key.key()).toString();

    return qt_key;
}

/**
 * Don't automatically close the dialog window when the escape key is pressed.
 * The user might want to bind "ESC" to an Apple II scan code.
 *
 * @param key The even describing the key that was pressed.
 */
void SettingsDialog::keyPressEvent(QKeyEvent *key)
{
    if(_waiting_for_press &&
       key->key() != Qt::Key_Shift &&
       key->key() != Qt::Key_Control &&
       key->key() != Qt::Key_Alt &&
       key->key() != Qt::Key_Meta &&
       key->key() != Qt::Key_Mode_switch)
    {
        if(_key_map.count(*key) == 0)
        {
            _ui->keyMapTable->item(_selected_row, 2)->setText(key_tostring(*key));
            _key_map.erase(_selected_keyevent);
            _key_map.emplace(*key, _selected_scancode);

            _ui->keyLineEdit->setText(key_tostring(*key));
            _ui->changeKeyBtn->setEnabled(true);
            _ui->keyMapTable->setSelectionMode(QAbstractItemView::SingleSelection);
            _waiting_for_press = false;
        }
        else
        {
            QMessageBox::warning(this,
                                 "Key Already Bound",
                                 "That key is already bound to another "
                                 "mapping. Please enter a key that isn't "
                                 "already mapped.");
        }
    }
    else
    {
        QDialog::keyPressEvent(key);
    }
}

/**
 * Runs whenever a new item is selected in the key mappings window.
 */
void SettingsDialog::on_keyMapTable_itemSelectionChanged()
{
    QTableWidgetItem *item = _ui->keyMapTable->item(_ui->keyMapTable->currentRow(), 2);
    _ui->keyLineEdit->setText(item->text());
    _ui->changeKeyBtn->setEnabled(true);
}

/**
 * Triggered when the "Change Key" button is pressed.
 */
void SettingsDialog::on_changeKeyBtn_clicked()
{
    QTableWidgetItem *item = _ui->keyMapTable->item(_ui->keyMapTable->currentRow(),0);
    const int target_row = item->data(1).toInt();

    int row = 0;
    auto iter = _key_map.begin();
    for(; iter != _key_map.end() && row <= target_row; row++, iter++)
    {
        if(row == target_row)
        {
            _selected_keyevent = iter->first;
            _selected_scancode = iter->second;
            _selected_row = _ui->keyMapTable->currentRow();
        }
    }

    _ui->keyLineEdit->setText("Press a key combination...");
    _ui->changeKeyBtn->setEnabled(false);
    _ui->keyMapTable->setSelectionMode(QAbstractItemView::NoSelection);
    _waiting_for_press = true;

    _ui->keyLineEdit->setFocus();
}

/**
 * Triggered when the dialog is "accepted" (aka, the OK button is pressed).
 */
void SettingsDialog::on_buttonBox_accepted()
{
    _emu.SetMappings(_key_map);
    _emu.SetVideoTextColor(_text_color);
    _emu.SetSpeakerMute(_speaker_mute);
}

/**
 * Open up a color picker for the user to choose a new video text color.
 */
void SettingsDialog::on_newColorButton_clicked()
{
    QColor new_color = QColorDialog::getColor(_text_color,
                                              this,
                                              "Video Text Color");

    if(new_color.isValid())
    {
        _text_color = new_color;

        _ui->redEdit->setText("0x" +
                tr("%1").arg(_text_color.red(), 2, 16, QChar('0')));
        _ui->greenEdit->setText("0x" +
                tr("%1").arg(_text_color.green(), 2, 16, QChar('0')));
        _ui->blueEdit->setText("0x" +
                tr("%1").arg(_text_color.blue(), 2, 16, QChar('0')));
    }
}

/**
 * Triggered when the speaker mute enable radio button is triggered.
 *
 * @param checked The state of the speaker mute.
 */
void SettingsDialog::on_speakerEnable_toggled(bool checked)
{
    _speaker_mute = checked;
}

/**
 * Destructor.
 */
SettingsDialog::~SettingsDialog()
{
    delete _ui;
}
