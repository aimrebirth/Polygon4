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

#include "SDBToolTableView.h"

#include "SDBToolTreeView.h"

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
        .TableView(this)
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
    TableView = InArgs._TableView;
    Item = InItem;

    SMultiColumnTableRow<ListItem>::Construct(FSuperRowType::FArguments(), InOwnerTable);
}

TSharedRef<SWidget> SButtonRowWidget::GenerateWidgetForColumn(const FName& ColumnName)
{
    using namespace polygon4::detail;

    using cb_item_type = TSharedPtr<ComboBoxItem>;
    using cb_item_array = TArray<cb_item_type>;
    using cb_item_array_ptr = TSharedPtr<cb_item_array>;
    using cb_type = SComboBox<cb_item_type>;

    auto &table = Item->Data->object->getClass();
    auto &var = Item->Var;
    auto Value = Item->Data->object->getVariableString(var.getId());
    TSharedPtr<SWidget> Widget;

    if (ColumnName == "Name")
    {
        auto dn = var.getDisplayName();
        auto s = polygon4::tr(dn);
        SAssignNew(Widget, STextBlock)
            .Text(s.toFText())
            ;
    }
    else if (ColumnName == "Type")
    {
        auto type = FText::FromString(polygon4::tr(var.getType()->getDataType()));
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
            TSharedPtr<SWidget> EditableTextBox;
            SAssignNew(EditableTextBox, SEditableTextBox)
                .IsEnabled(enabled)
                .RevertTextOnEscape(true)
                .Text(Value.toFText())
                .OnTextCommitted_Lambda([Item = Item](const FText &NewText, ETextCommit::Type Type)
            {
                try
                {
                    Item->Data->object->setVariableString(Item->Var.getId(), NewText);
                    Item->Data->update();
                    Item->Item->UpdateName();
                    GDBToolModule->SetDataChanged();
                }
                catch (std::exception &e)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Cannot parse value of type '%s': %s"),
                        ToCStr(polygon4::tr(Item->Var.getDataType()).toFText().ToString()),
                        ToCStr(polygon4::String(e.what()).toFText().ToString())
                        );
                }
            })
                ;
            return EditableTextBox;
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
                bool set = false;
                OrderedObjectMap m;
                std::tie(set, m) = Item->Data->object->getOrderedObjectMap(var.getId(), TableView->Storage);

                cb_item_array_ptr Items = MakeShareable(new cb_item_array);
                cb_item_type SelectedItem;

                bool found = false;
                for (auto &v : m)
                {
                    Items->Add(MakeShareable(new ComboBoxItem{
                        FText::FromString(v.first),
                        v.second
                    }));
                    if (!found && Value == v.second->getName())
                    {
                        SelectedItem = Items->Last();
                        found = true;
                    }
                }
                if (!found)
                {
                    Items->Add(MakeShareable(new ComboBoxItem{ FText::GetEmpty(), nullptr }));
                    SelectedItem = Items->Last();
                }

                TSharedRef<cb_type> cb = SNew(cb_type)
                    .OptionsSource(Items.Get())
                    .InitiallySelectedItem(SelectedItem)
                    .OnSelectionChanged_Lambda([ParentData = Item.Get(), Items, var, this, Data = Item->Data]
                        (const cb_item_type &InItem, ESelectInfo::Type type)
                {
                    auto data = ParentData->Data;
                    auto cb_data = InItem->Object;
                    if (cb_data)
                    {
                        data->object->setVariableString(ParentData->Var.getId(), cb_data);
                        data->update();
                        ParentData->Item->UpdateName();
                        TableView->RefreshTable(ParentData->Item);
                        GDBToolModule->SetDataChanged();
                    }
                })
                    .OnGenerateWidget_Lambda([](const cb_item_type &InItem)
                {
                    return
                        SNew(STextBlock)
                        .Text(InItem->Text);
                })
                    .Content()
                    [
                        SNew(STextBlock)
                        .Text(SelectedItem->Text)
                    ]
                ;

                Widget = StaticCastSharedRef<SWidget>(cb);
            }
        }
        else if (var.getDataType() == DataType::Bool)
        {
            TSharedRef<SCheckBox> cb = SNew(SCheckBox)
                .OnCheckStateChanged_Lambda([var, Data = Item->Data](ECheckBoxState state)
            {
                Data->object->setVariableString(var.getId(), state == ECheckBoxState::Checked ? "1" : "0");
            });
            cb->SetIsChecked(Value == "1" ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

            Widget = cb;
            return SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(10, 1)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    Widget.ToSharedRef()
                ];
        }
        else if (var.getDataType() == DataType::Enum)
        {
            cb_item_array_ptr Items = MakeShareable(new cb_item_array);
            cb_item_type SelectedItem;

            auto n = var.getType()->getCppName();
            int val = std::stoi(Value.toString());
            auto m = getOrderedMap(n);
            bool found = false;
            for (auto &v : m)
            {
                Items->Add(MakeShareable(new ComboBoxItem{
                    v.first.toFText(),
                    (polygon4::detail::IObjectBase *)(uint64_t)v.second
                }));
                if (val == v.second)
                {
                    SelectedItem = Items->Last();
                    found = true;
                }
            }
            if (!found)
            {
                Items->Add(MakeShareable(new ComboBoxItem{ FText::GetEmpty(), nullptr }));
                SelectedItem = Items->Last();
            }

            TSharedRef<cb_type> cb = SNew(cb_type)
                .OptionsSource(Items.Get())
                .InitiallySelectedItem(SelectedItem)
                .OnSelectionChanged_Lambda([ParentData = Item.Get(), Items, var, this, Data = Item->Data]
                    (const cb_item_type &Item, ESelectInfo::Type type)
            {
                int cb_data = (uint64_t)Item->Object;
                Data->object->setVariableString(var.getId(), std::to_string(cb_data));
                Data->update();
                ParentData->Item->UpdateName();
                TableView->RefreshTable(ParentData->Item);
                GDBToolModule->SetDataChanged();
            })
                .OnGenerateWidget_Lambda([](const cb_item_type &InItem)
            {
                return
                    SNew(STextBlock)
                    .Text(InItem->Text);
            })
                .Content()
                [
                    SNew(STextBlock)
                    .Text(SelectedItem->Text)
                ]
            ;

            Widget = StaticCastSharedRef<SWidget>(cb);
        }
        else if (var.hasFlags({ fBigEdit }))
        {
            TSharedRef<SMultiLineEditableTextBox> tb = SNew(SMultiLineEditableTextBox)
                .Text(Value.toFText())
                .AutoWrapText(true)
                .OnTextCommitted_Lambda([ParentData = Item.Get(), var, this, Data = Item->Data]
                    (const FText &text, ETextCommit::Type type)
            {
                Data->object->setVariableString(var.getId(), text);
                Data->name = Data->object->getName();
                ParentData->Item->UpdateName();
                TableView->RefreshTable(ParentData->Item);
                GDBToolModule->SetDataChanged();
            })
                ;
            Widget = StaticCastSharedRef<SWidget>(tb);
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

#undef LOCTEXT_NAMESPACE
