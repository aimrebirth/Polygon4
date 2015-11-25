/*
* Polygon-4
* Copyright (C) 2015 lzwdgc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DBToolPrivatePCH.h"
#include "SDBToolTextComboBox.h"
#include "SDBToolTableView.h"
#include "SDBToolTreeView.h"

void SDBToolTextComboBox::Construct(const FArguments& InArgs)
{
    ParentData = InArgs._ParentData;
    TableView = InArgs._TableView;

    ParentType::FArguments args;
    args
        .OptionsSource(&Items)
        .OnGenerateWidget(this, &SDBToolTextComboBox::OnGenerateWidget)
        .OnSelectionChanged(this, &SDBToolTextComboBox::OnSelectionChanged)
        ;
    ParentType::Construct(args);
}

TSharedRef<SWidget> SDBToolTextComboBox::OnGenerateWidget(ListItem InItem)
{
    return
        SNew(STextBlock)
        .Text(InItem->Text);
}

void SDBToolTextComboBox::OnSelectionChanged(ListItem InItem, ESelectInfo::Type SelectInfo)
{
    this->ButtonContentSlot->operator[]
    (
        SNew(STextBlock)
        .Text(InItem->Text)
    );
    
    if (!Initialized)
        return;

    auto data = ParentData->Data;
    auto cb_data = InItem->Object;
    if (cb_data)
    {
        bool same = data->object->getVariableString(ParentData->Var.getId()) == cb_data->getName();
        data->object->setVariableString(ParentData->Var.getId(), cb_data);
        data->update();
        ParentData->Item->UpdateName();
        TableView->RefreshTable(ParentData->Item);
        if (!same)
            GDBToolModule->SetDataChanged();
    }
}
