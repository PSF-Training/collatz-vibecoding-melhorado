#include <wx/wx.h>

#include <charconv>
#include <sstream>
#include <string>
#include <system_error>

#include "collatz/collatz.hpp"

namespace {

/// Parses one whitespace-separated token into a positive Collatz start value.
bool parse_start_token(const std::string& token, collatz::value_type& out) {
  collatz::value_type value = 0;
  const char* begin = token.data();
  const char* end = token.data() + token.size();
  const auto parsed = std::from_chars(begin, end, value);
  if (parsed.ec != std::errc{} || parsed.ptr != end || value == 0) {
    return false;
  }
  out = value;
  return true;
}

/// Minimal top-level window: type numbers, press Generate, read the sequences.
///
/// This is a deliberate placeholder. The detailed interface is deferred to a later
/// specification; the only job here is to prove the wxWidgets build integration.
class MainFrame : public wxFrame {
 public:
  MainFrame()
      : wxFrame(nullptr, wxID_ANY, "Collatz Explorer", wxDefaultPosition, wxSize(760, 560)) {
    auto* panel = new wxPanel(this);
    auto* layout = new wxBoxSizer(wxVERTICAL);

    input_ = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_PROCESS_ENTER);
    input_->SetHint("Starting values, e.g. 6 27 97");
    layout->Add(input_, 0, wxEXPAND | wxALL, 8);

    auto* generate = new wxButton(panel, wxID_ANY, "Generate");
    layout->Add(generate, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    output_ = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
    layout->Add(output_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    panel->SetSizer(layout);

    generate->Bind(wxEVT_BUTTON, &MainFrame::on_generate, this);
    input_->Bind(wxEVT_TEXT_ENTER, &MainFrame::on_generate, this);
    input_->SetFocus();
  }

 private:
  /// Renders every valid token in the input field, reporting invalid ones in place.
  void on_generate(wxCommandEvent&) {
    std::istringstream fields{std::string(input_->GetValue().ToUTF8())};
    std::ostringstream rendered;
    std::string token;
    while (fields >> token) {
      collatz::value_type start = 0;
      if (!parse_start_token(token, start)) {
        rendered << "error: '" << token << "' is not a positive integer\n";
        continue;
      }
      const collatz::Sequence sequence = collatz::generate(start);
      const collatz::Statistics stats = collatz::analyse(start);
      rendered << sequence.start << ": steps=" << stats.steps << " max=" << stats.max_value
               << " stopping_time=" << stats.stopping_time;
      if (stats.status != collatz::Status::Ok) {
        rendered << " status=" << collatz::to_string(stats.status);
      }
      rendered << '\n';
      for (std::size_t i = 0; i < sequence.values.size(); ++i) {
        if (i != 0) {
          rendered << ' ';
        }
        rendered << sequence.values[i];
      }
      rendered << "\n\n";
    }
    output_->SetValue(wxString::FromUTF8(rendered.str()));
  }

  wxTextCtrl* input_ = nullptr;
  wxTextCtrl* output_ = nullptr;
};

/// wx entry point; the detailed interface is defined in a later specification.
class CollatzApp : public wxApp {
 public:
  bool OnInit() override {
    (new MainFrame())->Show();
    return true;
  }
};

}  // namespace

wxIMPLEMENT_APP(CollatzApp);
