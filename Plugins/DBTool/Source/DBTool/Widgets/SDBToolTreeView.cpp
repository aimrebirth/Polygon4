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
#include "SDBToolTreeView.h"
#include "SDBToolTableView.h"

TSharedPtr<TreeItem> TreeItem::AddChild(const SPtrP4TreeItem &P4Item)
{
    TSharedPtr<TreeItem> Item = MakeShareable(new TreeItem());
    Item->P4Item = P4Item;
    Item->UpdateName();
    Item->Parent = this;
    Item->Build(P4Item.get());
    Children.Add(Item);
    return Item;
}

void TreeItem::Build(P4TreeItem *Root)
{
    if (Root->children.empty())
        return;
    for (auto &c : Root->children)
        AddChild(c);
    Children.Sort([](const auto &i1, const auto &i2) { return i1->Name.CompareTo(i2->Name) < 0; });
}

void SDBToolTreeView::Construct(const FArguments& InArgs)
{
    P4RootItem = InArgs._RootItem;
    TableView = InArgs._TableView;
    Storage = InArgs._Storage;
    RootItem = MakeShareable(new TreeItem());
    RootItem->Build(P4RootItem.get());

    ParentType::FArguments args;
    args
        .SelectionMode(ESelectionMode::Single)
        .ClearSelectionOnClick(false)
        .OnSelectionChanged(this, &SDBToolTreeView::OnSelectionChanged)
        .OnGenerateRow(this, &SDBToolTreeView::OnGenerateRow)
        .OnGetChildren(this, &SDBToolTreeView::OnGetChildren)
        ;
    if (RootItem.IsValid())
    {
        args
            .TreeItemsSource(&RootItem->Children)
            ;
    }
    ParentType::Construct(args);
}

void SDBToolTreeView::Reset(SPtrP4TreeItem P4RootItem)
{
    this->P4RootItem = P4RootItem;
    RootItem = MakeShareable(new TreeItem());
    RootItem->Build(P4RootItem.get());
    TreeItemsSource = &RootItem->Children;
    ReGenerateItems(PanelGeometryLastTick);
    RequestTreeRefresh();
}

TSharedRef<ITableRow> SDBToolTreeView::OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<ListItem>, OwnerTable)
        [
            SAssignNew(InItem->Widget, STextBlock)
            .Text(InItem->Name)
        ];
}

void SDBToolTreeView::OnGetChildren(ListItem Item, TArray<ListItem>& OutChildren)
{
    OutChildren.Append(Item->Children);
}

void SDBToolTreeView::OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo)
{
    if (!Item.IsValid() || !Item->P4Item->object)
    {
        TableView->ResetTable();
        return;
    }
    TableView->RefreshTable(Item.Get());
    Item->UpdateName();
}

void SDBToolTreeView::AddRecord()
{
    auto Items = GetSelectedItems();
    if (Items.Num() == 0)
        return;
    auto Item0 = Items[0];
    if (!Item0.IsValid())
        return;
    auto Item = Item0.Get();
    while (1)
    {
        if (Item->P4Item && Item->P4Item->object)
            Item = Item->Parent;
        else
            break;
    }
    auto r = Storage->addRecord(Item->P4Item.get());
    auto NewItem = Item->AddChild(r);
    auto index = Item->Children.IndexOfByKey(NewItem);
    GenerateWidgetForItem(NewItem, index, 0, 1.f);
    auto ParentItem = NewItem->Parent;
    while (ParentItem)
    {
        SetItemExpansion(MakeShareable(ParentItem, [](auto o) {}), true);
        ParentItem = ParentItem->Parent;
    }
    SetItemExpansion(NewItem, true);
    SetSelection(NewItem, ESelectInfo::OnMouseClick);
    GDBToolModule->SetDataChanged();
}

void SDBToolTreeView::DeleteRecord()
{
    auto Items = GetSelectedItems();
    if (Items.Num() == 0)
        return;
    auto Item0 = Items[0];
    if (!Item0.IsValid())
        return;
    auto Item = Item0.Get();
    if (!Item->P4Item || !Item->P4Item->object)
        return;
    Storage->deleteRecord(Item->P4Item.get());
    Item->P4Item->remove();
    auto Parent = Item->Parent;
    if (Parent)
    {
        auto index = Parent->Children.IndexOfByKey(Item0);
        Parent->Children.Remove(Item0);
        auto n = Parent->Children.Num();
        if (n != 0 && index == n)
            index--;
        if (n != 0)
            SetSelection(Parent->Children[index], ESelectInfo::OnMouseClick);
        else
            SetSelection(MakeShareable(Parent, [](auto o) {}), ESelectInfo::OnMouseClick);
    }
    RequestTreeRefresh();
    GDBToolModule->SetDataChanged();
}
