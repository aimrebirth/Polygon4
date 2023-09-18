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

#include "InfoTreeView.h"

#include "Game/P4Engine.h"
#include "UI/Colors.h"

#define LOCTEXT_NAMESPACE "InfoTreeView"

TSharedPtr<InfoTreeItem> InfoTreeItem::AddChild(P4InfoTreeItem *P4ItemIn)
{
    TSharedPtr<InfoTreeItem> Item = MakeShareable(new InfoTreeItem());
    Item->P4Item = P4ItemIn;
    Item->Parent = this;
    Item->Build(P4ItemIn);
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

void InfoTreeView::Reset(P4InfoTreeItem *P4RootItemIn)
{
    P4RootItem = P4RootItemIn;
    RootItem = MakeShareable(new InfoTreeItem());
    RootItem->Build(P4RootItem);
    SetTreeItemsSource(&RootItem->Children);
    //TreeItemsSource = &RootItem->Children;
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
    using namespace polygon4;
    using namespace polygon4::detail;

    FSlateColor Color = FLinearColor::White;
    if (InItem->P4Item->highlight)
        Color = FSlateColor(P4_COLOR_YELLOW);

    auto T = InItem->P4Item->text;
    auto o = InItem->P4Item->object;
    if (o)
    {
        switch (o->getType())
        {
        case EObjectType::JournalRecord:
        {
            auto r = (JournalRecord *)o;
            if (r->message)
            {
                polygon4::String s = r->message->title->string;
                T = s.toFText();
            }
            Color = FSlateColor(P4_COLOR_YELLOW);
            if (r->type == QuestRecordType::Completed)
                Color = FSlateColor(P4_COLOR_GRAY);
            break;
        }
        case EObjectType::ConfigurationProjectile:
        {
            auto p = (ConfigurationProjectile *)o;
            T += u" (";
            T += std::to_string(p->quantity);
            T += u")"s;
            break;
        }
        case EObjectType::MapBuildingGood:
        {
            auto p = (MapBuildingGood *)o;
            if (p->quantity != -1)
            {
                T += u" (";
                T += std::to_string(p->quantity);
                T += u")"s;
            }
            break;
        }
        default:
            break;
        }
    }

    auto FT = T.toFText();
    if (FT.IsEmpty())
        FT = LOCTEXT("EmptyString", "Empty");

    return SNew(STableRow<ListItem>, OwnerTable)
        [
            SAssignNew(InItem->Widget, STextBlock)
            .Text(FT)
            .Font(FSlateFontInfo(RobotoFont, 16))
            .ColorAndOpacity(Color)
        ];
}

void InfoTreeView::OnGetChildren(ListItem Item, TArray<ListItem>& OutChildren)
{
    OutChildren.Append(Item->Children);
}

void InfoTreeView::OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo)
{
    if (GetVisibility() != EVisibility::Visible)
        return;
    if (!Item.IsValid() || !Item->P4Item)
        return;
    auto bm = GP4Engine()->getBuildingMenu();
    auto p = Item->P4Item;
    if (!p->object)
    {
        bm->getText() = p->text;
        return;
    }
    switch (p->object->getType())
    {
    case polygon4::detail::EObjectType::Message:
        bm->showMessage((polygon4::detail::Message*)p->object);
        break;
    case polygon4::detail::EObjectType::JournalRecord:
        bm->showMessage(((polygon4::detail::JournalRecord*)p->object)->message);
        break;

#define PRINT_DESCRIPTION(t) \
    case polygon4::detail::EObjectType::t: \
        if (((polygon4::detail::t*)p->object)->description) \
            bm->showText(p->object->getName(), ((polygon4::detail::t*)p->object)->description->string); \
        else \
            bm->showText(p->object->getName()); \
        break

        PRINT_DESCRIPTION(Building);

        PRINT_DESCRIPTION(Glider);
        PRINT_DESCRIPTION(Good);
        PRINT_DESCRIPTION(Equipment);
        PRINT_DESCRIPTION(Weapon);
        PRINT_DESCRIPTION(Projectile);
        PRINT_DESCRIPTION(Modificator);

#define PRINT_DESCRIPTION_VAR(t, v) \
    case polygon4::detail::EObjectType::t: \
    do { \
        auto o = (polygon4::detail::t*)p->object; \
        if (o->v->description) \
            bm->showText(o->getName(), o->v->description->string); \
        else \
            bm->showText(o->getName()); \
        break; \
    } while (0)

        PRINT_DESCRIPTION_VAR(ConfigurationEquipment, equipment);
        PRINT_DESCRIPTION_VAR(ConfigurationGood, good);
        PRINT_DESCRIPTION_VAR(ConfigurationProjectile, projectile);
        PRINT_DESCRIPTION_VAR(ConfigurationWeapon, weapon);
        PRINT_DESCRIPTION_VAR(ConfigurationModificator, modificator);

        PRINT_DESCRIPTION_VAR(MapBuildingGlider, glider);
        PRINT_DESCRIPTION_VAR(MapBuildingEquipment, equipment);
        PRINT_DESCRIPTION_VAR(MapBuildingGood, good);
        PRINT_DESCRIPTION_VAR(MapBuildingProjectile, projectile);
        PRINT_DESCRIPTION_VAR(MapBuildingWeapon, weapon);
        PRINT_DESCRIPTION_VAR(MapBuildingModificator, modificator);
    }
}

static InfoTreeView::ListItem FindFirstParent(const InfoTreeView::ListItem &Root)
{
    if (Root->P4Item && Root->P4Item->highlight)
        return Root;
    for (auto &c : Root->Children)
    {
        auto p = FindFirstParent(c);
        if (p.IsValid())
            return p;
    }
    return nullptr;
}

static InfoTreeView::ListItem FindFirstNotParent(const InfoTreeView::ListItem &Root)
{
    if (Root->P4Item && !Root->P4Item->highlight)
        return Root;
    for (auto &c : Root->Children)
    {
        auto p = FindFirstNotParent(c);
        if (p.IsValid())
            return p;
    }
    return nullptr;
}

bool InfoTreeView::SelectFirstNotParent()
{
    auto f = [this](auto &&f)
    {
        auto i = f(RootItem);
        if (i.IsValid())
        {
            SetSelection(i, ESelectInfo::Direct);
            return true;
        }
        return false;
    };
    return f(FindFirstNotParent) || f(FindFirstParent);
}
