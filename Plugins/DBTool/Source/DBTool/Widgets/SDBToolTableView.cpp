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
#include "SDBToolTableView.h"
#include "SDBToolTreeView.h"
#include "SDBToolTextComboBox.h"

#include <algorithm>

#include <Polygon4/DataManager/Common.h>
#include <Polygon4/DataManager/StorageImpl.h>

#define LOCTEXT_NAMESPACE "DBTool"

void SDBToolTableView::Construct(const FArguments& InArgs)
{
    Storage = InArgs._Storage;

    ParentType::FArguments args;
    args
        .SelectionMode(ESelectionMode::Single)
        .ListItemsSource(&Data)
        .OnGenerateRow(this, &SDBToolTableView::OnGenerateRow)
        .HeaderRow(
            SNew(SHeaderRow)
            .Visibility(EVisibility::Hidden)
            //
            + SHeaderRow::Column("Name")
            .DefaultLabel(LOCTEXT("DBToolNameLabel", "Name"))
            .IsEnabled(true)
            .FillWidth(0.35)
            .VAlignHeader(EVerticalAlignment::VAlign_Center)
            .HAlignHeader(EHorizontalAlignment::HAlign_Center)
            .VAlignCell(EVerticalAlignment::VAlign_Center)
            .HAlignCell(EHorizontalAlignment::HAlign_Left)
            //
            + SHeaderRow::Column("Type")
            .DefaultLabel(LOCTEXT("DBToolTypeLabel", "Type"))
            .IsEnabled(false)
            .FillWidth(0.25)
            .VAlignHeader(EVerticalAlignment::VAlign_Center)
            .HAlignHeader(EHorizontalAlignment::HAlign_Center)
            .VAlignCell(EVerticalAlignment::VAlign_Center)
            .HAlignCell(EHorizontalAlignment::HAlign_Left)
            //
            + SHeaderRow::Column("Value")
            .DefaultLabel(LOCTEXT("DBToolValueLabel", "Value"))
            .IsEnabled(true)
            .FillWidth(0.4)
            .VAlignHeader(EVerticalAlignment::VAlign_Center)
            .HAlignHeader(EHorizontalAlignment::HAlign_Center)
            .VAlignCell(EVerticalAlignment::VAlign_Center)
            .HAlignCell(EHorizontalAlignment::HAlign_Fill)
            )
        ;
    ParentType::Construct(args);
}

TSharedRef<ITableRow> SDBToolTableView::OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return
        SNew(SButtonRowWidget, OwnerTable, InItem)
        .Storage(Storage)
        ;
}

void SDBToolTableView::ResetTable()
{
    Data.Empty();
    ClearSelection();
    HeaderRow->SetVisibility(EVisibility::Hidden);
    ReGenerateItems(PanelGeometryLastTick);
    RequestListRefresh();
}

void SDBToolTableView::RefreshTable(TreeItem *Item)
{
    auto &table = Item->P4Item->object->getClass();
    TArray<ListItem> NewData;
    for (auto &v : table.getVariables())
        NewData.Add(MakeShareable(new RowData{ Item, Item->P4Item.get(), v }));
    NewData.Sort([](const auto &i1, const auto i2) { return i1->Var.getId() < i2->Var.getId(); });
    Data = NewData;

    ClearSelection();
    HeaderRow->SetVisibility(EVisibility::Visible);
    ReGenerateItems(PanelGeometryLastTick);
    RequestListRefresh();
}

void SButtonRowWidget::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, ListItem InItem)
{
    Storage = InArgs._Storage;
    Item = InItem;

    SMultiColumnTableRow<ListItem>::Construct(FSuperRowType::FArguments(), InOwnerTable);
}

TSharedRef<SWidget> SButtonRowWidget::GenerateWidgetForColumn(const FName& ColumnName)
{
    auto &table = Item->Data->object->getClass();
    auto &var = Item->Var;
    auto value = Item->Data->object->getVariableString(var.getId());
    auto Value = FText::FromString(to_wstring(value).c_str());
    TSharedPtr<SWidget> Widget;

    if (ColumnName == "Name")
    {
        SAssignNew(Widget, STextBlock)
            .Text(FText::FromString(var.getName().c_str()))
            ;
    }
    else if (ColumnName == "Type")
    {
        auto type = var.isFk() ? FText::GetEmpty() : FText::FromString(to_wstring(polygon4::tr(var.getType()->getDataType())).c_str());
        SAssignNew(Widget, STextBlock)
            .Text(type)
            ;
    }
    else if (ColumnName == "Value")
    {
        bool enabled = true;
        if (var.isId())
            enabled = false;

        auto EditableTextBox = [&]()
        {
            TSharedPtr<SWidget> ValidatedEditableTextBox;
            SAssignNew(ValidatedEditableTextBox, SValidatedEditableTextBox)
                .IsEnabled(enabled)
                .Item(Item);
            return ValidatedEditableTextBox;
        };

        if (var.isFk())
        {
            if (table.getParent() && var.getType()->getCppName() == table.getParent()->getCppName())
            {
                enabled = false;
                Widget = EditableTextBox();
            }
            else
            {
                using namespace polygon4::detail;

                auto table_type = Item->Data->object->getType();
                auto type = Item->Data->object->getVariableType(var.getId());

                polygon4::detail::OrderedObjectMap m;

                // prevent printing objects from other maps
                auto f = [](auto m, auto mo) { return m->map == mo->map; };
                auto f2 = [](auto m, auto mo) { return m->modification == mo->modification && m->map == mo->map; };
                if (type == EObjectType::MapBuilding)
                    m = getOrderedMapForObject<MapBuilding>(Storage, (Mechanoid *)Item->Data->object, f);
                else if (type == EObjectType::MapGood)
                    m = getOrderedMapForObject<MapGood>(Storage, (Mechanoid *)Item->Data->object, f);
                else if (type == EObjectType::MapObject)
                    m = getOrderedMapForObject<MapObject>(Storage, (Mechanoid *)Item->Data->object, f);
                else if (type == EObjectType::ModificationMap)
                    m = getOrderedMapForObject<ModificationMap>(Storage, (Mechanoid *)Item->Data->object, f2);
                else
                    m = Storage->getOrderedMap(type);

                if (type == EObjectType::String)
                {
                    for (auto it = m.cbegin(); it != m.cend(); )
                    {
                        polygon4::detail::String *s = (polygon4::detail::String *)it->second;
                        if (s->object != table_type|| s->object == EObjectType::Any)
                            m.erase(it++);
                        else
                            ++it;
                    }
                }
                
                TSharedRef<SDBToolTextComboBox> cb = SNew(SDBToolTextComboBox).ParentData(Item.Get());
                auto &Items = cb->Items;

                bool found = false;
                for (auto &v : m)
                {
                    Items.Add(MakeShareable(new ComboBoxItem{
                        FText::FromString(to_wstring(v.first).c_str()),
                        v.second
                    }));
                    if (!found && value == v.second->getName())
                    {
                        cb->SetSelectedItem(Items.Last());
                        found = true;
                    }
                }
                if (!found)
                {
                    Items.Add(MakeShareable(new ComboBoxItem{ FText::GetEmpty(), nullptr }));
                    cb->SetSelectedItem(Items.Last());
                }
                Widget = StaticCastSharedRef<SWidget>(cb);
            }
        }
        /*else if (var.getDataType() == DataType::Bool)
        {
            auto chkb = new QCheckBox;
            chkb->setChecked(value == "1");
            connect(chkb, &QCheckBox::stateChanged, [chkb, var, data](int state)
            {
                data->object->setVariableString(var.getId(), chkb->isChecked() ? "1" : "0");
            });

            QWidget* wdg = new QWidget;
            QHBoxLayout layout(wdg);
            layout.addSpacing(3);
            layout.addWidget(chkb);
            layout.setAlignment(Qt::AlignLeft);
            layout.setMargin(0);
            wdg->setLayout(&layout);

            tableWidget->setCellWidget(var.getId(), col_id++, wdg);
            tableWidget->repaint();
        }
        else if (var.getDataType() == DataType::Enum)
        {
            QComboBox *cb = new QComboBox;
            auto n = var.getType()->getCppName();
            int val = std::stoi(to_string(value).c_str());
            auto m = getOrderedMap(n);
            bool found = false;
            for (auto &v : m)
            {
                cb->addItem(v.first.toQString(), (uint64_t)v.second);
                if (val == v.second)
                {
                    cb->setCurrentIndex(cb->count() - 1);
                    found = true;
                }
            }
            if (!found)
            {
                cb->addItem("");
                cb->setCurrentIndex(cb->count() - 1);
            }
            connect(cb, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, [cb, var, this](int index)
            {
                auto data = (TreeItem *)currentTreeWidgetItem->data(0, Qt::UserRole).toULongLong();
                auto cb_data = cb->currentData().toInt();
                data->object->setVariableString(var.getId(), std::to_string(cb_data));
                data->name = data->object->getName();
                currentTreeWidgetItemChanged(currentTreeWidgetItem, 0);
            });
            tableWidget->setCellWidget(var.getId(), col_id, cb);
        }*/
        else
        {
            Widget = EditableTextBox();
        }
    }
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(10, 1)
        [
            Widget.ToSharedRef()
        ];
}

void SValidatedEditableTextBox::Construct(const FArguments& InArgs)
{
    Item = InArgs._Item;

    auto value = Item->Data->object->getVariableString(Item->Var.getId());
    auto Value = FText::FromString(to_wstring(value).c_str());

    ParentType::FArguments args;
    args
        //.SelectAllTextWhenFocused(true)
        .RevertTextOnEscape(true)
        .OnTextCommitted(this, &SValidatedEditableTextBox::OnTextCommited)
        .OnKeyDownHandler(this, &SValidatedEditableTextBox::OnKeyDownHandler)
        .Text(Value);
    ParentType::Construct(args);
}

void SValidatedEditableTextBox::OnTextCommited(const FText &NewText, ETextCommit::Type Type)
{
    try
    {
        Item->Data->object->setVariableString(Item->Var.getId(), NewText.ToString().GetCharArray().GetData());
        Item->Data->update();
        Item->Item->UpdateName();
        GDBToolModule->SetDataChanged();
    }
    catch (std::exception &e)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot parse value of type '%s': %s"),
            to_string(polygon4::tr(Item->Var.getType()->getDataType())).c_str(),
            e.what());
    }
}

FReply SValidatedEditableTextBox::OnKeyDownHandler(const FGeometry &Geometry, const FKeyEvent &KeyEvent)
{
    auto c = KeyEvent.GetCharacter();
    switch (Item->Var.getDataType())
    {
    case DataType::Integer:
        if (!isdigit(c))
            return FReply::Unhandled();
        break;
    case DataType::Real:
        if (!isdigit(c) || c != '.')
            return FReply::Unhandled();
        break;
    default:
        break;
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
