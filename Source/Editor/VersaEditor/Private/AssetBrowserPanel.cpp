#include "U

#include "Runtime/Core/Public/Core/UICore.h"I/Editor/Panels/AssetBrowserPanel.h"
#include "UI/Framework/Widgets/UIInputWidgets.h"
#include "UI/Framework/Layout/UILayout.h"
#include "UI/Framework/Events/UIDragDropEvent.h"
#include <algorithm>
#include <filesystem>
#include <cctype>

namespace VGE {
namespace Editor {

namespace fs = std::filesystem;

AssetBrowserPanel::AssetBrowserPanel()
    : EditorPanel("Asset Browser")
    , m_ViewMode(ViewMode::Grid)
    , m_ThumbnailSize(100.0f)
    , m_SortField(SortField::Name)
    , m_SortAscending(true)
    , m_IsRenamingItem(false)
    , m_RenamingIndex(0)
    , m_CurrentHistoryIndex(-1)
{
    m_RenameBuffer[0] = '\0';
}

void AssetBrowserPanel::Initialize()
{
    EditorPanel::Initialize();
    
    // Set default root path to the project's assets directory
    SetRootPath("Assets");
}

void AssetBrowserPanel::Update(float deltaTime)
{
    EditorPanel::Update(deltaTime);
}

void AssetBrowserPanel::SetRootPath(const std::string& path)
{
    m_RootPath = path;
    m_CurrentPath = path;
    m_NavigationHistory.clear();
    m_NavigationHistory.push_back(path);
    m_CurrentHistoryIndex = 0;
    
    Refresh();
}

void AssetBrowserPanel::Refresh()
{
    LoadAssetsInCurrentPath();
}

void AssetBrowserPanel::NavigateToPath(const std::string& path)
{
    if (path == m_CurrentPath)
        return;

    m_CurrentPath = path;
    
    // Clear forward history if we're navigating from a back state
    if (m_CurrentHistoryIndex < m_NavigationHistory.size() - 1)
    {
        m_NavigationHistory.erase(m_NavigationHistory.begin() + m_CurrentHistoryIndex + 1,
                                m_NavigationHistory.end());
    }
    
    m_NavigationHistory.push_back(path);
    m_CurrentHistoryIndex = m_NavigationHistory.size() - 1;
    
    Refresh();
}

void AssetBrowserPanel::NavigateUp()
{
    fs::path current(m_CurrentPath);
    if (current.has_parent_path() && current != m_RootPath)
    {
        NavigateToPath(current.parent_path().string());
    }
}

void AssetBrowserPanel::CreateFolder(const std::string& name)
{
    fs::path newFolder = fs::path(m_CurrentPath) / name;
    fs::create_directory(newFolder);
    Refresh();
}

void AssetBrowserPanel::DeleteSelected()
{
    for (size_t index : m_SelectedIndices)
    {
        if (index < m_Assets.size())
        {
            fs::remove_all(m_Assets[index].path);
        }
    }
    
    m_SelectedIndices.clear();
    Refresh();
}

void AssetBrowserPanel::RenameSelected(const std::string& newName)
{
    if (m_SelectedIndices.size() != 1)
        return;

    size_t index = m_SelectedIndices[0];
    if (index >= m_Assets.size())
        return;

    fs::path oldPath(m_Assets[index].path);
    fs::path newPath = oldPath.parent_path() / newName;
    
    try
    {
        fs::rename(oldPath, newPath);
        Refresh();
    }
    catch (const std::exception& e)
    {
        // TODO: Show error message
    }
}

void AssetBrowserPanel::ImportAsset(const std::string& sourcePath)
{
    fs::path source(sourcePath);
    fs::path dest = fs::path(m_CurrentPath) / source.filename();
    
    try
    {
        fs::copy(source, dest, fs::copy_options::overwrite_existing);
        Refresh();
    }
    catch (const std::exception& e)
    {
        // TODO: Show error message
    }
}

const AssetItem* AssetBrowserPanel::GetSelectedAsset() const
{
    if (m_SelectedIndices.size() != 1)
        return nullptr;

    size_t index = m_SelectedIndices[0];
    return index < m_Assets.size() ? &m_Assets[index] : nullptr;
}

std::vector<const AssetItem*> AssetBrowserPanel::GetSelectedAssets() const
{
    std::vector<const AssetItem*> result;
    for (size_t index : m_SelectedIndices)
    {
        if (index < m_Assets.size())
        {
            result.push_back(&m_Assets[index]);
        }
    }
    return result;
}

void AssetBrowserPanel::SetFilter(const std::string& filter)
{
    m_Filter = filter;
}

void AssetBrowserPanel::SetTypeFilter(const std::string& type)
{
    m_TypeFilter = type;
}

void AssetBrowserPanel::RenderPanelContent()
{
    RenderToolbar();
    AddSeparator();
    RenderBreadcrumbs();
    AddSeparator();

    // Main content area
    switch (m_ViewMode)
    {
        case ViewMode::Grid:
            RenderAssetGrid();
            break;
        case ViewMode::List:
            RenderAssetList();
            break;
        case ViewMode::Details:
            RenderAssetDetails();
            break;
    }

    HandleDragAndDrop();
    RenderContextMenu();
}

void AssetBrowserPanel::LoadAssetsInCurrentPath()
{
    m_Assets.clear();
    m_SelectedIndices.clear();

    try
    {
        for (const auto& entry : fs::directory_iterator(m_CurrentPath))
        {
            AssetItem item;
            item.name = entry.path().filename().string();
            item.path = entry.path().string();
            item.isDirectory = entry.is_directory();
            
            if (item.isDirectory)
            {
                item.type = "Folder";
                item.icon = "folder";
            }
            else
            {
                item.type = entry.path().extension().string();
                item.icon = GetAssetIcon(item.type);
                GeneratePreview(item);
            }

            m_Assets.push_back(item);
        }
    }
    catch (const std::exception& e)
    {
        // TODO: Show error message
    }

    SortAssets();
}

void AssetBrowserPanel::SortAssets()
{
    auto comparator = [this](const AssetItem& a, const AssetItem& b) {
        // Folders always come first
        if (a.isDirectory != b.isDirectory)
            return a.isDirectory;

        int result = 0;
        switch (m_SortField)
        {
            case SortField::Name:
                result = a.name.compare(b.name);
                break;
            case SortField::Type:
                result = a.type.compare(b.type);
                break;
            // TODO: Implement other sort fields
        }

        return m_SortAscending ? (result < 0) : (result > 0);
    };

    std::sort(m_Assets.begin(), m_Assets.end(), comparator);
}

bool AssetBrowserPanel::FilterAsset(const AssetItem& asset) const
{
    if (!m_Filter.empty())
    {
        std::string lowerName = asset.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        std::string lowerFilter = m_Filter;
        std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
        
        if (lowerName.find(lowerFilter) == std::string::npos)
            return false;
    }

    if (!m_TypeFilter.empty() && !asset.isDirectory)
    {
        if (asset.type != m_TypeFilter)
            return false;
    }

    return true;
}

void AssetBrowserPanel::RenderToolbar()
{
    auto layout = CreateHorizontalLayout();
    
    if (auto button = CreateButton("<"))
    {
        if (m_CurrentHistoryIndex > 0)
        {
            m_CurrentHistoryIndex--;
            m_CurrentPath = m_NavigationHistory[m_CurrentHistoryIndex];
            Refresh();
        }
    }

    if (auto button = CreateButton(">"))
    {
        if (m_CurrentHistoryIndex < m_NavigationHistory.size() - 1)
        {
            m_CurrentHistoryIndex++;
            m_CurrentPath = m_NavigationHistory[m_CurrentHistoryIndex];
            Refresh();
        }
    }

    if (auto button = CreateButton("^"))
    {
        NavigateUp();
    }

    if (auto button = CreateButton("Refresh"))
    {
        Refresh();
    }

    const char* viewModes[] = { "Grid", "List", "Details" };
    int currentMode = static_cast<int>(m_ViewMode);
    if (auto combo = CreateComboBox("View", viewModes, 3, &currentMode))
    {
        m_ViewMode = static_cast<ViewMode>(currentMode);
    }

    if (auto slider = CreateSlider("Size", &m_ThumbnailSize, 50.0f, 200.0f))
    {
        // Thumbnail size updated automatically
    }

    if (auto input = CreateInputField("Filter", m_Filter))
    {
        SetFilter(m_Filter);
    }

    std::vector<std::string> typeFilters = { "All" };
    // Add unique types from assets
    for (const auto& asset : m_Assets)
    {
        if (std::find(typeFilters.begin(), typeFilters.end(), asset.type) == typeFilters.end())
        {
            typeFilters.push_back(asset.type);
        }
    }

    if (auto combo = CreateComboBox("Type", typeFilters, &m_TypeFilter))
    {
        // Type filter updated automatically
    }
}

void AssetBrowserPanel::RenderBreadcrumbs()
{
    auto layout = CreateHorizontalLayout();
    
    fs::path current(m_CurrentPath);
    fs::path relative = fs::relative(current, m_RootPath);
    
    if (auto text = CreateText(m_RootPath))
    {
        if (text->IsClicked())
        {
            NavigateToPath(m_RootPath);
        }
    }

    for (const auto& part : relative)
    {
        CreateText(">");
        if (auto button = CreateButton(part.string()))
        {
            fs::path targetPath = m_RootPath;
            for (const auto& p : fs::relative(part, relative))
            {
                targetPath /= p;
            }
            NavigateToPath(targetPath.string());
        }
    }
}

void AssetBrowserPanel::RenderAssetGrid()
{
    float availableWidth = GetContentWidth();
    int columnsCount = std::max(1, static_cast<int>(availableWidth / (m_ThumbnailSize + 20.0f)));
    
    auto grid = CreateGrid(columnsCount);
    
    for (size_t i = 0; i < m_Assets.size(); ++i)
    {
        const auto& asset = m_Assets[i];
        if (!FilterAsset(asset))
            continue;

        auto cell = grid->AddCell();
        bool isSelected = std::find(m_SelectedIndices.begin(), m_SelectedIndices.end(), i) != m_SelectedIndices.end();
        
        if (auto button = CreateImageButton(asset.previewPath, Vector2(m_ThumbnailSize)))
        {
            if (IsKeyPressed(Key::Control))
            {
                if (isSelected)
                {
                    m_SelectedIndices.erase(
                        std::remove(m_SelectedIndices.begin(), m_SelectedIndices.end(), i),
                        m_SelectedIndices.end()
                    );
                }
                else
                {
                    m_SelectedIndices.push_back(i);
                }
            }
            else
            {
                m_SelectedIndices.clear();
                m_SelectedIndices.push_back(i);
            }
        }

        if (m_IsRenamingItem && m_RenamingIndex == i)
        {
            if (auto input = CreateInputField("##rename", m_RenameBuffer, 256))
            {
                if (input->IsSubmitted())
                {
                    RenameSelected(m_RenameBuffer);
                    m_IsRenamingItem = false;
                }
            }
            
            if (!input->IsFocused() && (IsMouseClicked(MouseButton::Left) || IsMouseClicked(MouseButton::Right)))
            {
                m_IsRenamingItem = false;
            }
        }
        else
        {
            CreateText(asset.name);
        }

        // Drag and drop source
        if (auto dragDrop = CreateDragDropSource())
        {
            dragDrop->SetPayload("ASSET_PATH", asset.path);
            CreateText(asset.name);
        }
    }
}

void AssetBrowserPanel::RenderAssetList()
{
    auto table = CreateTable(3);
    table->AddColumn("Name");
    table->AddColumn("Type");
    table->AddColumn("Size");
    
    for (size_t i = 0; i < m_Assets.size(); ++i)
    {
        const auto& asset = m_Assets[i];
        if (!FilterAsset(asset))
            continue;

        bool isSelected = std::find(m_SelectedIndices.begin(), m_SelectedIndices.end(), i) != m_SelectedIndices.end();
        
        auto row = table->AddRow();
        
        if (auto cell = row->AddCell())
        {
            if (auto selectable = CreateSelectable(asset.name, isSelected))
            {
                if (IsKeyPressed(Key::Control))
                {
                    if (isSelected)
                    {
                        m_SelectedIndices.erase(
                            std::remove(m_SelectedIndices.begin(), m_SelectedIndices.end(), i),
                            m_SelectedIndices.end()
                        );
                    }
                    else
                    {
                        m_SelectedIndices.push_back(i);
                    }
                }
                else
                {
                    m_SelectedIndices.clear();
                    m_SelectedIndices.push_back(i);
                }
            }
        }
        
        row->AddCell()->CreateText(asset.type);
        
        std::string size = asset.isDirectory ? "" : std::to_string(fs::file_size(asset.path)) + " bytes";
        row->AddCell()->CreateText(size);
    }
}

void AssetBrowserPanel::RenderAssetDetails()
{
    // Similar to list view but with more columns and details
    if (ImGui::BeginTable("AssetDetails", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable))
    {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Date Modified");
        ImGui::TableSetupColumn("Path");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < m_Assets.size(); i++)
        {
            const auto& asset = m_Assets[i];
            if (!FilterAsset(asset))
                continue;

            ImGui::TableNextRow();
            
            bool isSelected = std::find(m_SelectedIndices.begin(), m_SelectedIndices.end(), i) != m_SelectedIndices.end();
            
            ImGui::PushID(static_cast<int>(i));
            
            // Name column
            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(asset.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (ImGui::GetIO().KeyCtrl)
                {
                    auto it = std::find(m_SelectedIndices.begin(), m_SelectedIndices.end(), i);
                    if (it != m_SelectedIndices.end())
                        m_SelectedIndices.erase(it);
                    else
                        m_SelectedIndices.push_back(i);
                }
                else
                {
                    m_SelectedIndices.clear();
                    m_SelectedIndices.push_back(i);
                }
            }

            // Type column
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", asset.type.c_str());

            // Size column
            ImGui::TableSetColumnIndex(2);
            if (!asset.isDirectory)
            {
                auto size = fs::file_size(asset.path);
                ImGui::Text("%lld B", size);
            }

            // Date modified column
            ImGui::TableSetColumnIndex(3);
            auto time = fs::last_write_time(asset.path);
            auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(time);
            auto timeT = std::chrono::system_clock::to_time_t(systemTime);
            ImGui::Text("%s", std::ctime(&timeT));

            // Path column
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%s", asset.path.c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void AssetBrowserPanel::RenderContextMenu()
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("New Folder"))
        {
            CreateFolder("New Folder");
        }

        if (ImGui::MenuItem("Import Asset..."))
        {
            // TODO: Open file dialog
        }

        ImGui::Separator();

        if (!m_SelectedIndices.empty())
        {
            if (m_SelectedIndices.size() == 1)
            {
                if (ImGui::MenuItem("Rename"))
                {
                    m_IsRenamingItem = true;
                    m_RenamingIndex = m_SelectedIndices[0];
                    strncpy_s(m_RenameBuffer, m_Assets[m_RenamingIndex].name.c_str(), sizeof(m_RenameBuffer) - 1);
                }
            }

            if (ImGui::MenuItem("Delete"))
            {
                DeleteSelected();
            }
        }

        ImGui::EndPopup();
    }
}

void AssetBrowserPanel::HandleDragAndDrop()
{
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
        {
            std::string path((const char*)payload->Data, payload->DataSize);
            ImportAsset(path);
        }
        ImGui::EndDragDropTarget();
    }
}

std::string AssetBrowserPanel::GetAssetIcon(const std::string& type) const
{
    // TODO: Implement asset type to icon mapping
    return "default";
}

void AssetBrowserPanel::GeneratePreview(AssetItem& asset)
{
    // TODO: Generate preview for supported asset types
}

} // namespace Editor
} // namespace VGE 


