// Core of the StormWar Tools

import java.io.*;
import java.util.*;

class Core
{
    Hashtable _plugins;

    public Core()
    {
        _plugins = new Hashtable();
    }



    public void processArgs(String[] args)
    {
        if (args.length == 0)
        {
            printUsage();
            return;
        }

        Plugin p = (Plugin)_plugins.get(args[0]);
        if (p == null)
        {
            System.err.println("Tool " + args[0] + " not found.");
            printUsage();
            return;
        }

        //plugin found, starting it
        String[] pargs = new String[args.length - 1];
        System.arraycopy(args, 1, pargs, 0, args.length - 1);
        p.processArgs(pargs);
    }



    public void addPlugin(Plugin plugin)
    {
        _plugins.put(plugin.getName(), plugin);
    }



    private void printUsage()
    {
        System.err.println("Arguments: <tool_name> [tool_args...]");
        System.err.println("List of tools:");
        for (Enumeration e = _plugins.keys(); e.hasMoreElements(); )
        {
            System.err.println("  " + e.nextElement());
        }
    }
}
