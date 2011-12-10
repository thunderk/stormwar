// StormWar Tools

import java.io.*;
import java.util.*;

class Main
{
    static private Core _core;

    static public void main(String[] args)
    {
        //init
        _core = new Core();
        _core.addPlugin(new UpdateI18N());

        //args processing
        _core.processArgs(args);
    }
}
