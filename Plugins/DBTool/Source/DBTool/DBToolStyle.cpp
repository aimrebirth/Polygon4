
#include "DBToolStyle.h"

#include "Projects.h"
#include "SlateBasics.h"

TSharedPtr< FSlateStyleSet > FDBToolStyle::StyleInstance = NULL;

void FDBToolStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FDBToolStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FDBToolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("DBToolStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

#define SET_ICON(button, filename) Style->Set("DBTool." #button, new IMAGE_BRUSH(TEXT(filename), Icon40x40))

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FDBToolStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("DBToolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("DBTool")->GetBaseDir() / TEXT("Resources"));

    SET_ICON(OpenPluginWindow, "polygon4_40");
    SET_ICON(SaveDB, "save");
    SET_ICON(ReloadDB, "reload");
    SET_ICON(AddRecord, "plus");
    SET_ICON(DeleteRecord, "minus");

	return Style;
}

#undef SET_ICON

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FDBToolStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FDBToolStyle::Get()
{
	return *StyleInstance;
}
