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

#include "Polygon4.h"
#include "InfoTreeView.h"

TSharedPtr<InfoTreeItem> InfoTreeItem::AddChild(P4InfoTreeItem *P4Item)
{
    TSharedPtr<InfoTreeItem> Item = MakeShareable(new InfoTreeItem());
    Item->P4Item = P4Item;
    Item->Parent = this;
    Item->Build(P4Item);
    Children.Add(Item);
    return Item;
}

void InfoTreeItem::Build(P4InfoTreeItem *Root)
{
    if (!Root)
        return;
    if (Root->children.empty())
        return;
    for (auto &c : Root->children)
        AddChild(c.get());
}

void InfoTreeView::Construct(const FArguments& InArgs)
{
    P4RootItem = InArgs._RootItem;
    RootItem = MakeShareable(new InfoTreeItem());
    RootItem->Build(P4RootItem);

    ParentType::FArguments args;
    args
        .SelectionMode(ESelectionMode::Single)
        .ClearSelectionOnClick(false)
        .OnSelectionChanged(this, &InfoTreeView::OnSelectionChanged)
        .OnGenerateRow(this, &InfoTreeView::OnGenerateRow)
        .OnGetChildren(this, &InfoTreeView::OnGetChildren)
        ;
    if (RootItem.IsValid())
    {
        args
            .TreeItemsSource(&RootItem->Children)
            ;
    }
    ParentType::Construct(args);

    SetExpandedItems(RootItem->Children);
}

void InfoTreeView::Reset(P4InfoTreeItem *P4RootItem)
{
    this->P4RootItem = P4RootItem;
    RootItem = MakeShareable(new InfoTreeItem());
    RootItem->Build(P4RootItem);
    TreeItemsSource = &RootItem->Children;
    SetExpandedItems(RootItem->Children);
    ReGenerateItems(PanelGeometryLastTick);
    RequestTreeRefresh();
}

void InfoTreeView::SetExpandedItems(const TArray<TSharedPtr<InfoTreeItem>> &Items)
{
    for (auto &i : Items)
    {
        SetItemExpansion(i, i->P4Item->expanded);
        SetExpandedItems(i->Children);
    }
}

TSharedRef<ITableRow> InfoTreeView::OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<ListItem>, OwnerTable)
        [
            SAssignNew(InItem->Widget, STextBlock)
            .Text(InItem->P4Item->text.toFText())
            .Font(FSlateFontInfo("Tahoma", 16))
        ];
}

void InfoTreeView::OnGetChildren(ListItem Item, TArray<ListItem>& OutChildren)
{
    OutChildren.Append(Item->Children);
}

void InfoTreeView::OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo)
{
    if (!Item.IsValid())
        return;
}
