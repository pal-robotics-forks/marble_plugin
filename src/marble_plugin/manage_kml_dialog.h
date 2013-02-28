// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

/* -- BEGIN LICENSE BLOCK ----------------------------------------------

Copyright (c) 2013, TB
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by TB.  The name of the
TB may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  -- END LICENSE BLOCK ----------------------------------------------*/

//----------------------------------------------------------------------
/*!\file
*
* \author  Tobias Bär <baer@fzi.de> Jan Aidel <aiden@fzi.de>
* \date    2013-01-11
*
*/
//----------------------------------------------------------------------


#ifndef MANAGE_KML_DIALOG_H
#define MANAGE_KML_DIALOG_H

#include <QDialog>
#include "qt4/Qt/qfileinfo.h"
#include <map>

namespace Ui {
class manageKmlDialog;
}

namespace marble_plugin{


class ManageKmlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ManageKmlDialog(std::map< QString, bool> &kml_files, QWidget *parent = 0);
    ~ManageKmlDialog();

    std::map< QString, bool> getKmlFiles();

    
    void addAllKMLsToTreeWiev(std::map< QString, bool>& kml_files);
private Q_SLOTS:
    void SetKMLFile(bool envoke_file_dialog = true );
    void deleteKML();
    void checkBoxClicked(int state);
    void cancelButtonClicked();
    void okButtonClicked();

private:
    void addKMLToTreeWiev(QFileInfo &kmlFile, bool show);

    Ui::manageKmlDialog *ui;
    std::map< QString, bool> m_kml_files;

};

}

#endif // MANAGE_KML_DIALOG_H
