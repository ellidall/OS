#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

const int ID_Add = 1;
const int ID_Delete = 2;

class TodoFrame : public wxFrame
{
public:
    const std::string DEFAULT_FILE_PATH = "../../task8_3/";
    const std::string DEFAULT_FILE_NAME = "tasks.txt";
    const std::string LOCK_FILE_PATH = "tasks.lock";

    explicit TodoFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
    {
        m_taskList = new wxListBox(this, wxID_ANY);
        m_taskInput = new wxTextCtrl(this, wxID_ANY);
        m_addButton = new wxButton(this, ID_Add, "Add");
        m_deleteButton = new wxButton(this, ID_Delete, "Delete");

        auto* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_taskInput, 0, wxEXPAND | wxALL, 5);
        sizer->Add(m_addButton, 0, wxEXPAND | wxALL, 5);
        sizer->Add(m_taskList, 1, wxEXPAND | wxALL, 5);
        sizer->Add(m_deleteButton, 0, wxEXPAND | wxALL, 5);

        SetSizer(sizer);
        m_taskInput->Bind(wxEVT_TEXT, &TodoFrame::OnTextChange, this);
        AddStyles();
        LoadTasks();
    }

private:
    wxListBox* m_taskList;
    wxTextCtrl* m_taskInput;
    wxButton* m_addButton;
    wxButton* m_deleteButton;
    std::vector<std::string> m_tasks;
    wxString m_filePath = DEFAULT_FILE_PATH + DEFAULT_FILE_NAME;
    wxString m_lockedFilePath = DEFAULT_FILE_PATH + LOCK_FILE_PATH;

wxDECLARE_EVENT_TABLE();

    void AddStyles()
    {
        m_addButton->Enable(false);
        wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        m_addButton->SetFont(font);
        m_deleteButton->SetFont(font);

        m_addButton->SetBackgroundColour(*wxLIGHT_GREY);
        m_deleteButton->SetBackgroundColour(*wxLIGHT_GREY);
    }

    void OnAdd(wxCommandEvent& event)
    {
        wxString task = m_taskInput->GetValue();
        if (!task.IsEmpty())
        {
            m_taskList->Append(task);
            m_tasks.emplace_back(task);
            m_taskInput->Clear();
            m_addButton->Enable(false);
            SaveTasks();
        }
    }

    void OnDelete(wxCommandEvent& event)
    {
        int selection = m_taskList->GetSelection();
        if (selection != wxNOT_FOUND)
        {
            m_taskList->Delete(selection);
            m_tasks.erase(m_tasks.begin() + selection);
            SaveTasks();
        }
    }

    void OnTextChange(wxCommandEvent& event)
    {
        if (m_taskInput->GetValue().IsEmpty())
        {
            m_addButton->Enable(false);
        }
        else
        {
            m_addButton->Enable(true);
        }
    }

    void OnClose(wxCloseEvent& event)
    {
        std::remove(m_lockedFilePath.c_str());
        event.Skip();
    }

    void LoadTasks()
    {
        wxString filePath = wxFileSelector("Choose File", DEFAULT_FILE_PATH,
                DEFAULT_FILE_NAME, "txt",
                "Text files (*.txt)|*.txt|All files (*.*)|*.*",
                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (!filePath.IsEmpty())
        {
            m_filePath = filePath;
        }
        std::filesystem::path lockedFilePath = std::filesystem::path(m_filePath.ToStdString()).replace_extension(".lock");
        m_lockedFilePath = lockedFilePath.string();

        // решить проблему более надёжным способом, через эксклюзивное владение файлом
        // singleInstance в библиотеке
        // именованный мьютекс
        // boost gls

        if (std::filesystem::exists(lockedFilePath))
        {
            wxMessageBox("Another instance is running or the file is locked.", "Error", wxOK | wxICON_ERROR);
            m_lockedFilePath = "";
            Close();
            return;
        }

        std::ofstream lockFile(lockedFilePath);
        if (!lockFile.is_open())
        {
            wxMessageBox("Failed to create lock file.", "Error", wxOK | wxICON_ERROR);
            Close();
            return;
        }

        std::ifstream file(m_filePath.ToStdString());
        if (!file.is_open())
        {
            wxMessageBox("Failed to open file " + m_filePath, "Error", wxOK | wxICON_ERROR);
            Close();
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            {
                m_tasks.push_back(line);
                m_taskList->Append(line);
            }
        }
    }

    void SaveTasks()
    {
        std::ofstream file(m_filePath.ToStdString());
        for (const auto& task : m_tasks)
        {
            file << task << std::endl;
        }
    }
};

class TodoApp : public wxApp
{
public:
    bool OnInit() override
    {
        auto* frame = new TodoFrame("TODO List");
        frame->Show(true);
        return true;
    }
};

wxBEGIN_EVENT_TABLE(TodoFrame, wxFrame)
                EVT_BUTTON(ID_Add, TodoFrame::OnAdd)
                EVT_BUTTON(ID_Delete, TodoFrame::OnDelete)
                EVT_CLOSE(TodoFrame::OnClose)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(TodoApp)