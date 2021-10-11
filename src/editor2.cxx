#include <boost/format.hpp>

#include "mud.hxx"
#include "connection.hxx"

DECLARE_DO_FUN(do_test_new_editor);

class EditorShell : public Shell
{
    public:
    EditorShell(std::unique_ptr<Shell> parent, DESCRIPTOR_DATA& d) : Shell(std::move(parent), d, false)
    {
    }

    void handleCommand(DESCRIPTOR_DATA& desc, const std::string& command) override
    {
        std::string handy = "received: ";
        for (char c : command)
        {
            handy += (boost::format(" 0x%02x") % (int)c).str();
        }
        bug(handy.c_str());
     
        bool exitAtEnd = false;
        for (char c : command)
        {
            if (c == 0x18)
            {
                exitAtEnd = true;
            }
        }

        if (exitAtEnd)
            exit(desc);
    }

    void exit(DESCRIPTOR_DATA& desc)
    {
        bug("exiting editor...");
        auto self = restoreParentShell(desc);
    }
};

void do_test_new_editor(CHAR_DATA* ch, char* argument)
{
    if (ch->desc == nullptr || IS_NPC(ch))
    {
        bug("NPCs can't edit things");
        return;
    }

    ch->desc->shell = std::make_unique<EditorShell>(std::move(ch->desc->shell), *ch->desc);

    ch->desc->connection->setLineBuffered(false);
}