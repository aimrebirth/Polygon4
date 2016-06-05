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

#include "Widgets/MenuButton.h"
#include "Widgets/SavedGamesListView.h"
#include "Widgets/InfoTreeView.h"

#include <Game/P4Engine.h>
#include <UI/Colors.h>

#define LOCTEXT_NAMESPACE "BuildingMenu"

TextDecorator::TextDecorator(const FTextBlockStyle &DefaultStyle)
    : DefaultStyle(DefaultStyle)
{
}

bool TextDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    return
        RunInfo.Name == TEXT("span") ||
        RunInfo.Name == TEXT("ref") ||
        0;
}

TSharedRef<ISlateRun> TextDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
    FRunInfo RunInfo(RunParseResult.Name);
    for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
    {
        RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    return CreateRun(TextLayout, RunInfo, InOutModelText, CreateTextBlockStyle(RunInfo), ModelRange);
}

TSharedRef<ISlateRun> TextDecorator::CreateRun(const TSharedRef<FTextLayout>& TextLayout, const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange)
{
    return FSlateTextRun::Create(InRunInfo, InText, Style, InRange);
}

FTextBlockStyle TextDecorator::CreateTextBlockStyle(const FRunInfo& InRunInfo) const
{
    FSlateFontInfo Font;
    FLinearColor FontColor;
    ExplodeRunInfo(InRunInfo, Font, FontColor);

    FTextBlockStyle TextBlockStyle;
    TextBlockStyle.SetFont(Font);
    TextBlockStyle.SetColorAndOpacity(FontColor);

    return TextBlockStyle;
}

void TextDecorator::ExplodeRunInfo(const FRunInfo& InRunInfo, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const
{
    OutFont = DefaultStyle.Font;

    const FString* const FontFamilyString = InRunInfo.MetaData.Find(TEXT("font"));
    const FString* const FontSizeString = InRunInfo.MetaData.Find(TEXT("size"));
    const FString* const FontStyleString = InRunInfo.MetaData.Find(TEXT("style"));
    const FString* const FontColorString = InRunInfo.MetaData.Find(TEXT("color"));
    const FString* const ObjString = InRunInfo.MetaData.Find(TEXT("object"));
    const FString* const BldString = InRunInfo.MetaData.Find(TEXT("building"));

    if (FontFamilyString)
    {
        FStringAssetReference Font(**FontFamilyString);
        if (UObject* FontAsset = Font.TryLoad())
        {
            OutFont.FontObject = FontAsset;
        }
    }

    if (FontSizeString)
    {
        OutFont.Size = static_cast<uint8>(FPlatformString::Atoi(**FontSizeString));
    }

    if (FontStyleString)
    {
        OutFont.TypefaceFontName = FName(**FontStyleString);
    }

    OutFontColor = DefaultStyle.ColorAndOpacity.GetSpecifiedColor();
    if (FontColorString)
    {
        auto FontColorStringRef = FontColorString->ToLower();

        // Is Hex color?
        if (!FontColorStringRef.IsEmpty() && FontColorStringRef[0] == TCHAR('#'))
        {
            OutFontColor = FLinearColor(FColor::FromHex(FontColorStringRef));
        }
#define CHANGE_COLOR(c, cc) else if (FontColorStringRef == L ## #c) { OutFontColor = cc; }
        CHANGE_COLOR(green, P4_COLOR_GREEN)
        CHANGE_COLOR(blue, P4_COLOR_BLUE)
        CHANGE_COLOR(red, P4_COLOR_RED)
        CHANGE_COLOR(yellow, P4_COLOR_YELLOW)
        // add more text colors here
    }
    else
    {
        if (ObjString || BldString)
            OutFontColor = P4_COLOR_BLUE;
    }
}

void SBuildingMenu::Construct(const FArguments& InArgs)
{
    auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }

    auto BackgroundTexture = LoadObject<UTexture2D>(0, TEXT("Texture2D'/Game/Mods/Common/Images/bg_base.bg_base'"));
    FSlateBrush *BackgroundBrush;
    /*if (BackgroundTexture)
        BackgroundBrush = new FSlateDynamicImageBrush(BackgroundTexture, FVector2D{ 100,100 }, FName("bg_base"));
    else*/
        BackgroundBrush = new FSlateColorBrush(FColor::Black);

    LeftMenuButtons.resize(polygon4::bbMax);

    FTextBlockStyle FontStyle;
    FontStyle.SetFont(FSlateFontInfo("Tahoma", 14));
    FontStyle.SetColorAndOpacity(FLinearColor::White);

    TArray< TSharedRef< class ITextDecorator > > TextDecorators;
    TextDecorators.Add(MakeShareable(new TextDecorator(FontStyle)));

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
                                .Font(FSlateFontInfo("Tahoma", 14))
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
                                        + VerticalSlotMenuButton(LOCTEXT("TradeButton", "Trade"), this, &SBuildingMenu::DoNothing, &LeftMenuButtons[polygon4::bbTrade])
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
                                    SNew(SBorder)
                                    .Padding(10)
                                    [
                                        SNew(SVerticalBox)
                                        + SVerticalBox::Slot()
                                        [
                                            // for rich text: default text style, decorators
                                            SAssignNew(Text, TextWidget)
                                            .TextStyle(&FontStyle)
                                            .Decorators(TextDecorators)
                                            .Text(this, &SBuildingMenu::getFText)
                                            .AutoWrapText(true)
                                            //+ TextDecorator::Create(MakeShareable(new FTextLayout), )
                                        ]
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
                            + BottomText(LOCTEXT("RatingLabel", "Rating: "), RatingTB)
                            + BottomText(LOCTEXT("MassLabel", "Mass: "), MassTB)
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
				        .Font(FSlateFontInfo("Verdana", 100))
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
				                    .Font(FSlateFontInfo("Verdana", 30))
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
                                        .Font(FSlateFontInfo("Verdana", 30))
                                    ]
                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    [
                                        SAssignNew(SaveNameTB, SEditableTextBox)
                                        .RevertTextOnEscape(true)
                                        .Font(FSlateFontInfo("Verdana", 30))
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

SVerticalBox::FSlot& SBuildingMenu::BottomText(FText Name, TSharedPtr<STextBlock> &Var) const
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
				.Font(FSlateFontInfo("Verdana", 18))
				.Text(Name)
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
				.Font(FSlateFontInfo("Verdana", 18))
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

        auto conf = mechanoid->getConfiguration();
        auto m = std::to_string((int)conf->getMass());
        auto c = std::to_string((int)conf->getCapacity());
        m = m + "/" + c;
        MassTB->SetText(FString(m.c_str()));
    }
    ThemesTV->Reset(&themes);

    update();

    JournalTV->Reset(&journal);
    GliderTV->Reset(&glider);
    GliderStoreTV->Reset(&glider_store);
}

void SBuildingMenu::OnShow()
{
    GP4Engine()->UnsetPauseMenuBindings();
}

void SBuildingMenu::OnHide()
{
    auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = false;
    }
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
    BackRightVB->SetVisibility(EVisibility::Visible);
    JournalVB->SetVisibility(EVisibility::Visible);
    ThemesVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnGlider()
{
    BackLeftVB->SetVisibility(EVisibility::Visible);
    JournalVB->SetVisibility(EVisibility::Collapsed);
    GliderVB->SetVisibility(EVisibility::Visible);
    GliderStoreVB->SetVisibility(EVisibility::Visible);
    BackRightVB->SetVisibility(EVisibility::Collapsed);
    ButtonsVB->SetVisibility(EVisibility::Collapsed);
    ThemesVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnBack()
{
    BackLeftVB->SetVisibility(EVisibility::Collapsed);
    BackRightVB->SetVisibility(EVisibility::Collapsed);
    JournalVB->SetVisibility(EVisibility::Collapsed);
    GliderVB->SetVisibility(EVisibility::Collapsed);
    GliderStoreVB->SetVisibility(EVisibility::Collapsed);
    ButtonsVB->SetVisibility(EVisibility::Visible);
    ThemesVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}
