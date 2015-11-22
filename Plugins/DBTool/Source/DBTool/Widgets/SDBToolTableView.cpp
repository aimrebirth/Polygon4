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
#include <Polygon4/DataManager/Helpers.h>

#define LOCTEXT_NAMESPACE "DBTool"

FText getColumnTypeString(polygon4::ColumnType type)
{
    using polygon4::ColumnType;
    switch (type)
    {
    case ColumnType::Integer:
        return LOCTEXT("INTEGER", "Integer");
    case ColumnType::Real:
        return LOCTEXT("REAL", "Real");
    case ColumnType::Text:
        return LOCTEXT("TEXT", "Text");
    case ColumnType::Blob:
        return LOCTEXT("BLOB", "Blob");
    }
    return FText::GetEmpty();
}

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

void SDBToolTableView::RefreshTable(TreeItem *Item, const polygon4::DatabaseSchema *schema)
{
    auto &table = schema->tables.find(to_string(polygon4::detail::getTableNameByType(Item->P4Item->type)))->second;
    TArray<ListItem> NewData;
    for (auto &col : table.columns)
        NewData.Add(MakeShareable(new RowData{ Item, Item->P4Item.get(), schema, &table, &col.second }));
    NewData.Sort([](const auto &i1, const auto i2) { return i1->Column->id < i2->Column->id; });
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
    auto Column = Item->Column;
    auto value = Item->Data->object->getVariableString(Column->id);
    auto Value = FText::FromString(to_wstring(value).c_str());
    TSharedPtr<SWidget> Widget;

    if (ColumnName == "Name")
    {
        SAssignNew(Widget, STextBlock)
            .Text(FText::FromString(Column->name.c_str()))
            ;
    }
    else if (ColumnName == "Type")
    {
        auto type = Column->fk ? FText::GetEmpty() : getColumnTypeString(Column->type);
        SAssignNew(Widget, STextBlock)
            .Text(type)
            ;
    }
    else if (ColumnName == "Value")
    {
        bool enabled = true;
        if (Column->id == 0 && Column->name == "id" && Column->type == polygon4::ColumnType::Integer)
            enabled = false;

        auto EditableTextBox = [&]()
        {
            TSharedPtr<SWidget> ValidatedEditableTextBox;
            SAssignNew(ValidatedEditableTextBox, SValidatedEditableTextBox)
                .IsEnabled(enabled)
                .Item(Item);
            return ValidatedEditableTextBox;
        };

        if (Column->fk)
        {
            std::string s1 = removeId(Column->name);
            std::string s2 = Item->Table->name;
            std::transform(s1.begin(), s1.end(), s1.begin(), tolower);
            std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
            if (s2.find(s1) == 0)
            {
                enabled = false;
                Widget = EditableTextBox();
            }
            else
            {
                using namespace polygon4::detail;

                auto table_type = getTableType(Item->Table->name);
                auto type = getTableType(Column->fk->table_name);

                polygon4::detail::OrderedObjectMap m;

                // prevent printing objects from other maps
                auto f = [](auto m, auto mb) { return m->map == mb->map; };
                if (type == EObjectType::MapBuilding)
                    m = getOrderedMapForObject<MapBuilding>(Storage, (Mechanoid *)Item->Data->object, f);
                else if (type == EObjectType::MapGood)
                    m = getOrderedMapForObject<MapGood>(Storage, (Mechanoid *)Item->Data->object, f);
                else if (type == EObjectType::MapObject)
                    m = getOrderedMapForObject<MapObject>(Storage, (Mechanoid *)Item->Data->object, f);
                else
                    m = Storage->getOrderedMap(type);

                if (type == EObjectType::String)
                {
                    for (auto it = m.cbegin(); it != m.cend(); )
                    {
                        polygon4::detail::String *s = (polygon4::detail::String *)it->second;
                        if (s->table && (s->table->getId() != static_cast<int>(table_type) || s->table->getId() == static_cast<int>(EObjectType::Any)))
                            m.erase(it++);
                        else
                            ++it;
                    }
                }
                if (type == EObjectType::Table)
                {
                    decltype(m) m2;
                    for (auto it = m.cbegin(); it != m.cend(); it++)
                        m2.insert(std::make_pair(it->first, it->second));
                    m = m2;
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

    auto value = Item->Data->object->getVariableString(Item->Column->id);
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
        Item->Data->object->setVariableString(Item->Column->id, NewText.ToString().GetCharArray().GetData());
        Item->Data->update();
        Item->Item->UpdateName();
        GDBToolModule->SetDataChanged();
    }
    catch (std::exception &e)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot parse value of type '%s': %s"),
            getColumnTypeString(Item->Column->type).ToString().GetCharArray().GetData(),
            e.what());
    }
}

FReply SValidatedEditableTextBox::OnKeyDownHandler(const FGeometry &Geometry, const FKeyEvent &KeyEvent)
{
    using polygon4::ColumnType;
    auto c = KeyEvent.GetCharacter();
    switch (Item->Column->type)
    {
    case ColumnType::Integer:
        if (!isdigit(c))
            return FReply::Unhandled();
        break;
    case ColumnType::Real:
        if (!isdigit(c) || c != '.')
            return FReply::Unhandled();
        break;
    default:
        break;
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
