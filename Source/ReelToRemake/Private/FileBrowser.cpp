#include "FileBrowser.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#endif

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

TArray<FString> UFileDialogLibrary::OpenFilePanel(
    const FString& Title, 
    const FString& Directory, 
    const FString& Extension, 
    bool bMultiselect)
{
    FExtensionFilter Filter;
    Filter.Description = TEXT("Files");
    Filter.Extension = Extension;
    return OpenFilePanelMultiFilter(Title, Directory, { Filter }, bMultiselect);
}

TArray<FString> UFileDialogLibrary::OpenFilePanelMultiFilter(
    const FString& Title, 
    const FString& Directory, 
    const TArray<FExtensionFilter>& Extensions, 
    bool bMultiselect)
{
    TArray<FString> OutFiles;

#if WITH_EDITOR
    if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
    {
        FString FilterStr = "";
        for (const auto& Filter : Extensions)
        {
            FilterStr += FString::Printf(TEXT("%s (%s)|%s|"), *Filter.Description, *Filter.Extension, *Filter.Extension);
        }
        FilterStr.RemoveFromEnd(TEXT("|"));

        uint32 Flags = bMultiselect ? EFileDialogFlags::Multiple : EFileDialogFlags::None;

        DesktopPlatform->OpenFileDialog(
            nullptr,
            Title,
            Directory,
            TEXT(""),
            FilterStr,
            Flags,
            OutFiles
        );
    }
#elif PLATFORM_WINDOWS
    wchar_t szFile[4096] = { 0 };

    // Build Win32 filter string using TArray/concatenation to avoid Printf format sanitizer errors
    TArray<TCHAR> WinFilter;
    for (const auto& Filter : Extensions)
    {
        for (TCHAR C : Filter.Description) { WinFilter.Add(C); }
        WinFilter.Add(0);
        for (TCHAR C : Filter.Extension) { WinFilter.Add(C); }
        WinFilter.Add(0);
    }
    WinFilter.Add(0);

    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GEngine->GameViewport && GEngine->GameViewport->GetWindow() ? (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle() : nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = WinFilter.GetData();
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = Directory.IsEmpty() ? nullptr : *Directory;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (bMultiselect)
    {
        ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    }

    if (GetOpenFileNameW(&ofn) != 0)
    {
        wchar_t* p = ofn.lpstrFile;
        FString Folder = p;
        p += Folder.Len() + 1;

        if (*p == 0)
        {
            OutFiles.Add(Folder);
        }
        else
        {
            FString BaseDir = Folder;
            while (*p != 0)
            {
                FString FileName = p;
                OutFiles.Add(FPaths::Combine(BaseDir, FileName));
                p += FileName.Len() + 1;
            }
        }
    }
#endif

    return OutFiles;
}

FString UFileDialogLibrary::SaveFilePanel(
    const FString& Title, 
    const FString& Directory, 
    const FString& DefaultName, 
    const FString& Extension)
{
    FString OutFile = TEXT("");

#if WITH_EDITOR
    if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
    {
        FString FilterStr = FString::Printf(TEXT("Files (%s)|%s"), *Extension, *Extension);
        TArray<FString> SaveFiles;

        DesktopPlatform->SaveFileDialog(
            nullptr,
            Title,
            Directory,
            DefaultName,
            FilterStr,
            EFileDialogFlags::None,
            SaveFiles
        );

        if (SaveFiles.Num() > 0)
        {
            OutFile = SaveFiles[0];
        }
    }
#elif PLATFORM_WINDOWS
    wchar_t szFile[260] = { 0 };
    if (!DefaultName.IsEmpty())
    {
        // Replaced deprecated Strcpy with Strncpy
        FCString::Strncpy(szFile, *DefaultName, 260);
    }

    TArray<TCHAR> WinFilter;
    FString Desc = TEXT("Files");
    for (TCHAR C : Desc) { WinFilter.Add(C); }
    WinFilter.Add(0);
    for (TCHAR C : Extension) { WinFilter.Add(C); }
    WinFilter.Add(0);
    WinFilter.Add(0);

    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GEngine->GameViewport && GEngine->GameViewport->GetWindow() ? (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle() : nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = WinFilter.GetData();
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = Directory.IsEmpty() ? nullptr : *Directory;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameW(&ofn) != 0)
    {
        OutFile = FString(ofn.lpstrFile);
        FString CleanExt = Extension.Replace(TEXT("*"), TEXT(""));
        if (!OutFile.EndsWith(CleanExt) && !Extension.Contains(TEXT("*")))
        {
            OutFile += CleanExt;
        }
    }
#endif

    return OutFile;
}

TArray<FString> UFileDialogLibrary::OpenFolderPanel(
    const FString& Title, 
    const FString& Directory, 
    bool bMultiselect)
{
    TArray<FString> OutFolders;

#if WITH_EDITOR
    if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
    {
        FString OutFolderName;
        if (DesktopPlatform->OpenDirectoryDialog(nullptr, Title, Directory, OutFolderName))
        {
            OutFolders.Add(OutFolderName);
        }
    }
#elif PLATFORM_WINDOWS
    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = *Title;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl != nullptr)
    {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, path))
        {
            OutFolders.Add(FString(path));
        }
        CoTaskMemFree(pidl);
    }
#endif

    return OutFolders;
}