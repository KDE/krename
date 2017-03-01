/***************************************************************************
                       exthistorycombo.h  -  description
                             -------------------
    begin                : Sun May 20 2007
    copyright            : (C) 2007 by Dominik Seichter
    email                : domseichter@web.de
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXT_HISTORY_COMBO_H
#define EXT_HISTORY_COMBO_H

#include <QTimer>
#include <KHistoryComboBox>

/** A combobox with an item history, additional context
 *  menu entries for KRename tokens and a delayed text input
 *  signal which is only emitted after the user has typed
 *  sevaral characters.
 */
class ExtHistoryCombo : public KHistoryComboBox {
 Q_OBJECT

 public:
    /**
     * Create a new ExtHistoryCombo object with a parent and a name.
     *
     * @param parent Parent widget
     */
    explicit ExtHistoryCombo( QWidget* parent );

    /// Fix compilation, designer needs this method
    inline void insertItems( int, QStringList & list ) { KHistoryComboBox::insertItems( list ); }

    void loadConfig();
    void saveConfig();

    /**
     * select the current contents of the combobox.
     */
    void selectAll();

 Q_SIGNALS:
    void delayedTextChanged();

 private Q_SLOTS:
     void slotTextChanged();

 private:
    QTimer m_timer;

};

#endif /* EXT_HISTORY_COMBO_H */

