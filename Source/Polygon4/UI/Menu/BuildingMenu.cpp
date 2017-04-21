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
#include "BuildingMenu.h"

#include <Polygon4/Mechanoid.h>

#include "TextDecorators.h"
#include "Widgets/MenuButton.h"
#include "Widgets/SavedGamesListView.h"
#include "Widgets/InfoTreeView.h"

#include <Game/P4Engine.h>
#include <UI/Colors.h>
#include <UI/Game/SBar.h>

#define LOCTEXT_NAMESPACE "BuildingMenu"

void SBuildingMenu::OnHyperlinkClick(const FSlateHyperlinkRun::FMetadata &Map)
{
    const FString* const IdString = Map.Find(TEXT("id"));
    const FString* const NameString = Map.Find(TEXT("name"));

    if (!IdString || !NameString)
        return;
    if (*IdString == TEXT("script"))
    {
        polygon4::ScriptParameters params;
        int n = 1;
        while (const FString* const ParamString = Map.Find((L"param" + std::to_wstring(n++)).c_str()))
            params.push_back(*ParamString);
        scriptCallback(polygon4::String(*NameString).toString(), params);
    }
    else
        addTheme(*NameString);

    // refresh themes tree view
    ThemesTV->Reset(&themes);
}

FSlateWidgetRun::FWidgetRunInfo SBuildingMenu::EditWidgetDecorator(const FTextRunInfo& RunInfo, const ISlateStyle* Style) const
{
    TSharedRef<SWidget> Widget = SNew(SEditableTextBox)
        .MinDesiredWidth(200)
        .Font(FSlateFontInfo(RobotoFont, 14))
        .OnTextCommitted_Lambda([this](const FText &NewText, ETextCommit::Type Type)
    {
        if (!mechanoid->setName(NewText))
            mechanoid->setName(L"");
    })
        ;
    return FSlateWidgetRun::FWidgetRunInfo(Widget, 0);
}

void SBuildingMenu::Construct(const FArguments& InArgs)
{
    if (auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController())
        PlayerController->bShowMouseCursor = true;

    auto BackgroundTexture = LoadObject<UTexture2D>(0, TEXT("Texture2D'/Game/Mods/Common/Images/bg_base.bg_base'"));
    FSlateBrush *BackgroundBrush;
    //if (BackgroundTexture)
    //    BackgroundBrush = new FSlateDynamicImageBrush(BackgroundTexture, FVector2D{ 100,100 }, FName("bg_base"));
    //else
        BackgroundBrush = new FSlateColorBrush(FColor::Black);

    LeftMenuButtons.resize(polygon4::bbMax);

    FTextBlockStyle FontStyle;
    FontStyle.SetFont(FSlateFontInfo(RobotoFont, 14));
    FontStyle.SetColorAndOpacity(FLinearColor(P4_COLOR_WHITE));

    FTextBlockStyle HyperlinkTextStyle;
    HyperlinkTextStyle.SetFont(FSlateFontInfo(RobotoFont, 14));
    HyperlinkTextStyle.SetColorAndOpacity(FLinearColor(P4_COLOR_BLUE));
    //HyperlinkStyle.SetHighlightColor(FLinearColor::White);

    FTextBlockStyle HyperlinkTextStyle2;
    HyperlinkTextStyle2.SetFont(FSlateFontInfo(RobotoFont, 14));
    HyperlinkTextStyle2.SetColorAndOpacity(FLinearColor(P4_COLOR_RED));

    // set hyperlink styles
    FSlateStyleSet &ss = (FSlateStyleSet&)FCoreStyle::Get();

    ss.Set<FTextBlockStyle>("p4blue", HyperlinkTextStyle);
    ss.Set<FTextBlockStyle>("p4red", HyperlinkTextStyle2);

    //ss.Set<FHyperlinkStyle>("p4blue_hl", HyperlinkStyle2);
    //ss.Set<FHyperlinkStyle>("p4red_hl", HyperlinkStyle2);
    //FHyperlinkStyle->UnderlineStyle.Normal = style_ptr->UnderlineStyle.Disabled;
    //FHyperlinkStyle->UnderlineStyle.Hovered = style_ptr->UnderlineStyle.Disabled;

    // set decorators
    TArray< TSharedRef< class ITextDecorator > > TextDecorators;
    // text
    TextDecorators.Add(MakeShareable(new TextDecorator(FontStyle)));

    // hyperlinks
    TextDecorators.Add(SRichTextBlock::HyperlinkDecorator("object", this, &SBuildingMenu::OnHyperlinkClick));
    TextDecorators.Add(SRichTextBlock::HyperlinkDecorator("script", this, &SBuildingMenu::OnHyperlinkClick));
    //FSlateHyperlinkRun::FOnClick::CreateSP( InUserObjectPtr, NavigateFunc )

    // widgets
    TextDecorators.Add(SRichTextBlock::WidgetDecorator("edit", this, &SBuildingMenu::EditWidgetDecorator));

    SAssignNew(RatingBar, SBar)
        .Max(100)
        .Current(100)
        .Text(FText::FromString("Rating"))
        .Color(FLinearColor::Blue * 0.65f)
        ;
    SAssignNew(MassBar, SBar)
        .Max(100)
        .Current(100)
        .Text(FText::FromString("Mass"))
        .Color(FLinearColor::Green * 0.65f);

    ChildSlot
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBorder)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Padding(10)
            .BorderImage(BackgroundBrush)
            [
                SNew(SVerticalBox)
                // building menu
                + SVerticalBox::Slot()
                [
                    SAssignNew(BuildingMenuVB, SVerticalBox)
                    // caption
                    + SVerticalBox::Slot()
                    .VAlign(VAlign_Top)
                    .HAlign(HAlign_Fill)
                    .AutoHeight()
                    [
                        SNew(SBorder)
                        .Padding(10)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .HAlign(HAlign_Center)
                            [
                                SAssignNew(Name, STextBlock)
                                .Font(FSlateFontInfo(RobotoFont, 14))
                                .Text(FText::FromString(L"Building Name"))
                            ]
                        ]
                    ]
                    // top layout
                    + SVerticalBox::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    [
                        SNew(SHorizontalBox)
                        // left menu
                        + SHorizontalBox::Slot()
                        .FillWidth(0.25)
                        [
                            SNew(SVerticalBox)
                            // part w/out back button
                            + SVerticalBox::Slot()
                            .HAlign(HAlign_Fill)
                            .VAlign(VAlign_Fill)
                            [
                                SNew(SBorder)
                                .Padding(10)
                                [
                                    SNew(SVerticalBox)
                                    // buttons
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(ButtonsVB, SVerticalBox)
                                        + VerticalSlotMenuButton(LOCTEXT("SaveButton", "Save"), this, &SBuildingMenu::OnSave, &LeftMenuButtons[polygon4::bbSave])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("JournalButton", "Journal"), this, &SBuildingMenu::OnJournal, &LeftMenuButtons[polygon4::bbJournal])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("GliderButton", "Glider"), this, &SBuildingMenu::OnGlider, &LeftMenuButtons[polygon4::bbGlider])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("TradeButton", "Trade"), this, &SBuildingMenu::OnTrade, &LeftMenuButtons[polygon4::bbTrade])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("ClansButton", "Clans"), this, &SBuildingMenu::DoNothing, &LeftMenuButtons[polygon4::bbClans])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("TunnelButton", "Tunnel"), this, &SBuildingMenu::DoNothing, &LeftMenuButtons[polygon4::bbTunnel])
                                        .VAlign(VAlign_Top)
                                        + VerticalSlotMenuButton(LOCTEXT("ExitButton", "Exit"), this, &SBuildingMenu::OnExit, &LeftMenuButtons[polygon4::bbExit])
                                        .VAlign(VAlign_Bottom)
                                    ]
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(GliderVB, SVerticalBox)
                                        .Visibility(EVisibility::Collapsed)
                                        + SVerticalBox::Slot()
                                        .HAlign(HAlign_Fill)
                                        .VAlign(VAlign_Fill)
                                        [
                                            SAssignNew(GliderTV, InfoTreeView)
                                            .RootItem(&glider)
                                        ]
                                    ]
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(HoldStoreVB, SVerticalBox)
                                        .Visibility(EVisibility::Collapsed)
                                        + SVerticalBox::Slot()
                                        .HAlign(HAlign_Fill)
                                        .VAlign(VAlign_Fill)
                                        [
                                            SAssignNew(HoldStoreTV, InfoTreeView)
                                            .RootItem(&hold_store)
                                        ]
                                    ]
                                ]
                            ]
                            // back button
                            + SVerticalBox::Slot()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Bottom)
                            .AutoHeight()
                            [
                                SAssignNew(BackLeftVB, SVerticalBox)
                                .Visibility(EVisibility::Collapsed)
                                + VerticalSlotMenuButton(LOCTEXT("BackButton", "Back"), this, &SBuildingMenu::OnBack)
                            ]
                        ]
                        // middle
                        + SHorizontalBox::Slot()
                        .FillWidth(0.5)
                        [
                            SNew(SBorder)
                            .Padding(10)
                            [
                                SNew(SVerticalBox)
                                // text
                                + SVerticalBox::Slot()
                                .VAlign(VAlign_Fill)
                                .HAlign(HAlign_Fill)
                                [
                                    SNew(SScrollBox)
                                    + SScrollBox::Slot()
                                    .VAlign(VAlign_Fill)
                                    .HAlign(HAlign_Fill)
                                    [
                                        // for rich text: default text style, decorators
                                        SAssignNew(Text, TextWidget)
                                        .TextStyle(&FontStyle)
                                        .Decorators(TextDecorators)
                                        .Text(this, &SBuildingMenu::getFText)
                                        .AutoWrapText(true)
                                    ]
                                ]
                            ]
                        ]
                        // right menu
                        + SHorizontalBox::Slot()
                        .FillWidth(0.25)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .HAlign(HAlign_Fill)
                            .VAlign(VAlign_Fill)
                            [
                                SNew(SBorder)
                                .Padding(10)
                                [
                                    SNew(SVerticalBox)
                                    // themes
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(ThemesVB, SVerticalBox)
                                        + SVerticalBox::Slot()
                                        [
                                            SAssignNew(ThemesTV, InfoTreeView)
                                            .RootItem(&themes)
                                        ]
                                    ]
                                    // journal
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(JournalVB, SVerticalBox)
                                        .Visibility(EVisibility::Collapsed)
                                        + SVerticalBox::Slot()
                                        [
                                            SAssignNew(JournalTV, InfoTreeView)
                                            .RootItem(&journal)
                                        ]
                                    ]
                                    // glider
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(GliderStoreVB, SVerticalBox)
                                        .Visibility(EVisibility::Collapsed)
                                        + SVerticalBox::Slot()
                                        [
                                            SAssignNew(GliderStoreTV, InfoTreeView)
                                            .RootItem(&glider_store)
                                        ]
                                    ]
                                    // goods
                                    + SVerticalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    .VAlign(VAlign_Fill)
                                    [
                                        SAssignNew(GoodsStoreVB, SVerticalBox)
                                        .Visibility(EVisibility::Collapsed)
                                        + SVerticalBox::Slot()
                                        [
                                            SAssignNew(GoodsStoreTV, InfoTreeView)
                                            .RootItem(&goods_store)
                                        ]
                                    ]
                                    // clans
                                ]
                            ]
                            // back button
                            + SVerticalBox::Slot()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Bottom)
                            .AutoHeight()
                            [
                                SAssignNew(BackRightVB, SVerticalBox)
                                .Visibility(EVisibility::Collapsed)
                                + VerticalSlotMenuButton(LOCTEXT("BackButton", "Back"), this, &SBuildingMenu::OnBack)
                            ]
                        ]
                    ]
                    // bottom layout (money, rating etc.)
                    + SVerticalBox::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Bottom)
                    .AutoHeight()
                    [
                        SNew(SBorder)
                        .Padding(10)
                        [
                            SNew(SVerticalBox)
                            + BottomText(LOCTEXT("MoneyLabel", "Money: "), MoneyTB)
                            + BottomText(LOCTEXT("RatingLabel", "Rating: "), RatingTB, RatingBar)
                            + BottomText(LOCTEXT("MassLabel", "Mass: "), MassTB, MassBar)
                        ]
                    ]
                ]
                // load menu
                + SVerticalBox::Slot()
                [
                    SAssignNew(SaveVB, SVerticalBox)
                    .Visibility(EVisibility::Collapsed)
                    // Label
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Top)
			        .HAlign(HAlign_Center)
                    .AutoHeight()
			        [
				        SNew(STextBlock)
				        .ShadowColorAndOpacity(FLinearColor::Black)
				        .ColorAndOpacity(FLinearColor::White)
				        .ShadowOffset(FIntPoint(-1, 1))
				        .Font(FSlateFontInfo(RobotoFont, 100))
				        .Text(FText::FromString("Polygon-4"))
			        ]
                    // Other
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Fill)
			        .HAlign(HAlign_Fill)
                    [
                        SNew(SHorizontalBox)
                        // Left part of the screen (buttons)
                        + SHorizontalBox::Slot()
			            .VAlign(VAlign_Center)
			            .HAlign(HAlign_Left)
                        .AutoWidth()
                        [
                            SNew(SVerticalBox)
                            + VerticalSlotMenuButton(LOCTEXT("SaveButtonLabel", "Save"), this, &SBuildingMenu::OnSaveSave)
                            + VerticalSlotMenuButton(LOCTEXT("BackButtonLabel", "Back"), this, &SBuildingMenu::OnSaveBack)
                            + VerticalSlotMenuButton(LOCTEXT("DeleteButtonLabel", "Delete"), this, &SBuildingMenu::OnSaveDelete)
                        ]
                        // right part of the screen
                        + SHorizontalBox::Slot()
			            .VAlign(VAlign_Fill)
			            .HAlign(HAlign_Fill)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            [
                                SNew(SVerticalBox)
                                // label
                                + SVerticalBox::Slot()
			                    .VAlign(VAlign_Top)
			                    .HAlign(HAlign_Center)
                                .Padding(20)
                                .AutoHeight()
                                [
				                    SNew(STextBlock)
				                    .ShadowColorAndOpacity(FLinearColor::Black)
				                    .ColorAndOpacity(FLinearColor::White)
				                    .ShadowOffset(FIntPoint(-1, 1))
				                    .Font(FSlateFontInfo(RobotoFont, 30))
				                    .Text(LOCTEXT("SavedGamesLabel", "Saved Games"))
                                ]
                                // List View
                                + SVerticalBox::Slot()
			                    .VAlign(VAlign_Fill)
			                    .HAlign(HAlign_Fill)
                                .Padding(20)
                                [
                                    SAssignNew(SavedGamesListView, SSavedGamesListView)
                                    .OnSelectionChanged_Lambda([this](SSavedGamesListView::ListItem Item, ESelectInfo::Type Type)
                                {
                                    if (!Item.IsValid())
                                        return;
                                    SaveNameTB->SetText(Item->Name);
                                })
                                ]
                                // save name edit
                                + SVerticalBox::Slot()
			                    .VAlign(VAlign_Top)
			                    .HAlign(HAlign_Fill)
                                .Padding(20)
                                [
                                    SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Left)
                                    .AutoWidth()
                                    [
                                        SNew(STextBlock)
                                        .Text(LOCTEXT("SaveNameLabel", "Save Name: "))
                                        .Font(FSlateFontInfo(RobotoFont, 30))
                                    ]
                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    [
                                        SAssignNew(SaveNameTB, SEditableTextBox)
                                        .RevertTextOnEscape(true)
                                        .Font(FSlateFontInfo(RobotoFont, 30))
                                    ]
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ;
}

SVerticalBox::FSlot& SBuildingMenu::BottomText(FText NameIn, TSharedPtr<STextBlock> &Var) const
{
    return
        SVerticalBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo(RobotoFont, 18))
				.Text(NameIn)
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SAssignNew(Var, STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo(RobotoFont, 18))
            ]
        ];
}

SVerticalBox::FSlot& SBuildingMenu::BottomText(FText NameIn, TSharedPtr<STextBlock> &Var, TSharedPtr<SBar> &Bar) const
{
    return
        SVerticalBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo(RobotoFont, 18))
				.Text(NameIn)
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SAssignNew(Var, STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo(RobotoFont, 18))
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(10, 0, 0, 0)
            //.AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(500)
                .HeightOverride(25)
                [
                    Bar.ToSharedRef()
                ]
            ]
        ];
}

void SBuildingMenu::refresh()
{
    if (building)
    {
        Name->SetText(building->getName().toFText());

        polygon4::BuildingButtonsBitset bbb((uint32_t)building->buttons);
        for (int i = 0; i < polygon4::bbMax; i++)
            LeftMenuButtons[i]->SetVisibility(bbb[i] ? EVisibility::Visible : EVisibility::Hidden);
    }
    if (mechanoid)
    {
        MoneyTB->SetText(FString(std::to_string((int)mechanoid->money).c_str()));

        auto rat = std::to_string((int)mechanoid->rating);
        int level = mechanoid->getRatingLevel();
        auto cap = polygon4::getRatingLevelCap(level);
        auto caps = std::to_string((int)cap);
        rat = rat + "/" + caps + " (Level: " + std::to_string(level) + ")";
        RatingTB->SetText(FString(rat.c_str()));

        RatingBar->SetMaxValue(cap);
        RatingBar->SetCurrentValue((int)mechanoid->rating);

        auto conf = mechanoid->getConfiguration();
        auto m = std::to_string((int)conf->getMass());
        auto c = std::to_string((int)conf->getCapacity());
        m = m + "/" + c;
        MassTB->SetText(FString(m.c_str()));

        MassBar->SetMaxValue((int)conf->getCapacity());
        MassBar->SetCurrentValue((int)conf->getMass());
    }

    update();

    ThemesTV->Reset(&themes);
    JournalTV->Reset(&journal);
    GliderTV->Reset(&glider);
    GliderStoreTV->Reset(&glider_store);
    HoldStoreTV->Reset(&hold_store);
    GoodsStoreTV->Reset(&goods_store);
}

void SBuildingMenu::OnShow()
{
    GP4Engine()->UnsetPauseMenuBindings();
}

void SBuildingMenu::OnHide()
{
    if (auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController())
        PlayerController->bShowMouseCursor = false;
    GP4Engine()->SetPauseMenuBindings();
}

FReply SBuildingMenu::OnSave()
{
    BuildingMenuVB->SetVisibility(EVisibility::Collapsed);
    SaveVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SBuildingMenu::OnExit()
{
    if (GET_SETTINGS().flags[polygon4::gfReplenishEnergyOnBuildingExit])
    {
        auto c = mechanoid->getConfiguration();
        c->energy = c->getMaxEnergy();
        c->energy_shield = c->getMaxEnergyShield();
    }

    OnBack();
    GP4Engine()->HideBuildingMenu();
    GP4Engine()->spawnCurrentPlayer();
    return FReply::Handled();
}

FText SBuildingMenu::getFText() const
{
    return getText().toFText();
}

FReply SBuildingMenu::OnSaveSave()
{
    auto n = SaveNameTB->GetText().ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    if (!GP4Engine()->save(n))
        return FReply::Unhandled();
    SavedGamesListView->ReloadSaves(true);
    return FReply::Handled();
}

FReply SBuildingMenu::OnSaveBack()
{
    BuildingMenuVB->SetVisibility(EVisibility::Visible);
    SaveVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnSaveDelete()
{
    auto selected = SavedGamesListView->GetSelectedItems();
    if (!selected.Num())
        return FReply::Unhandled();
    auto n = selected[0]->Name.ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    GP4Engine()->deleteSaveGame(n);
    SavedGamesListView->ReloadSaves(true);
    return FReply::Handled();
}

FReply SBuildingMenu::OnJournal()
{
    refresh();
    saveScreenText();
    if (!JournalTV->SelectFirstNotParent())
        clearText();

    BackRightVB->SetVisibility(EVisibility::Visible);
    JournalVB->SetVisibility(EVisibility::Visible);
    ThemesVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnGlider()
{
    refresh();
    saveScreenText();
    if (!GliderTV->SelectFirstNotParent())
        clearText();

    BackLeftVB->SetVisibility(EVisibility::Visible);
    JournalVB->SetVisibility(EVisibility::Collapsed);
    GliderVB->SetVisibility(EVisibility::Visible);
    GliderStoreVB->SetVisibility(EVisibility::Visible);
    BackRightVB->SetVisibility(EVisibility::Collapsed);
    ButtonsVB->SetVisibility(EVisibility::Collapsed);
    ThemesVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnTrade()
{
    refresh();
    saveScreenText();
    if (!HoldStoreTV->SelectFirstNotParent())
        clearText();

    BackLeftVB->SetVisibility(EVisibility::Visible);
    JournalVB->SetVisibility(EVisibility::Collapsed);
    HoldStoreVB->SetVisibility(EVisibility::Visible);
    GoodsStoreVB->SetVisibility(EVisibility::Visible);
    BackRightVB->SetVisibility(EVisibility::Collapsed);
    ButtonsVB->SetVisibility(EVisibility::Collapsed);
    ThemesVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnBack()
{
    loadScreenText();

    BackLeftVB->SetVisibility(EVisibility::Collapsed);
    BackRightVB->SetVisibility(EVisibility::Collapsed);
    JournalVB->SetVisibility(EVisibility::Collapsed);
    GliderVB->SetVisibility(EVisibility::Collapsed);
    GliderStoreVB->SetVisibility(EVisibility::Collapsed);
    HoldStoreVB->SetVisibility(EVisibility::Collapsed);
    GoodsStoreVB->SetVisibility(EVisibility::Collapsed);
    ButtonsVB->SetVisibility(EVisibility::Visible);
    ThemesVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}
