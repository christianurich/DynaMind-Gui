/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef GUIGROUPTEST_H
#define GUIGROUPTEST_H

#include <QDialog>

namespace DM {
class Module;
}

class GroupTest;
namespace Ui {
class GUIGroupTest;
}

class GUIGroupTest : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIGroupTest(DM::Module * m, QWidget *parent = 0);
    ~GUIGroupTest();
    
private:
    Ui::GUIGroupTest *ui;
    GroupTest * m;


protected slots:
    void addInPort();
    void addOutPort();
};

#endif // GUIGROUPTEST_H