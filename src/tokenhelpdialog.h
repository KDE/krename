/***************************************************************************
                       tokenhelpdialog.h  -  description
                             -------------------
    begin                : Mon Jul 30 2007
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

#ifndef _TOKEN_HELP_DIALOG_H_
#define _TOKEN_HELP_DIALOG_H_

#include <kdialog.h>

#include "ui_tokenhelpwidget.h"

class QLineEdit;
class QTreeWidgetItem;

class BatchRenamer;
class KRenameModel;
class KRenameUnformattedWrappedModel;

class TokenHelpDialog : public KDialog {
 Q_OBJECT
 public:
    TokenHelpDialog( KRenameModel* model, BatchRenamer* renamer, 
                     QLineEdit* edit, QWidget* parent = NULL );

    ~TokenHelpDialog();

    /** Adds a category to the help dialog.
     *  The category will be added and all its commands will also be added to the "All" category.
     *
     *  \param headline headline of the category
     *  \param commands list of all tokens
     *  \param icon icon of the category
     *  \param first if true make sure that this is the first category (after all) and is selected by 
     *         default.
     */
    void add( const QString & headline, const QStringList & commands, const QPixmap & icon, bool first = false );

    /**
     * Retrieve the separator between token and help
     * \returns the token separator
     */
    static const QString getTokenSeparator();

 public slots:
     int exec();

 private slots:
    void slotInsert();

    void slotCategoryChanged( QTreeWidgetItem* item );
    void saveConfig();

    void slotEnableControls();
    void slotPreviewClicked(bool bPreview);
    void slotUpdatePreview();

 private:
    static const char* S_TOKEN_SEPARATOR; ///< Separator between token and help

    void loadConfig();
    
    /**
     * Select a category by its name.
     * \param category user visible name of a category
     */
    void selectCategory( const QString & category );
    
    /**
     * Add all tokens stored in the list
     * m_recent to the list of tokens and to categories.
     */
    void addRecentTokens();

    /**
     * Add a token to the list of recent tokens.
     * \param token the token itself
     * \param help the help string for the token
     */
    void addToRecentTokens( const QString & token, const QString & help );

 private:
    KRenameUnformattedWrappedModel* m_model;

    Ui::TokenHelpWidget m_widget;

    QLineEdit*          m_edit;
    BatchRenamer*       m_renamer;

    QMap<QString,QStringList> m_map;

    QString             m_first;

    QString             m_lastSelected;
    QStringList         m_recent;

    static const int    S_MAX_RECENT; ///< Maximum number of recent tokens
};

#endif // _TOKEN_HELP_DIALOG_H_
