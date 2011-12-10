// Plugins for the StormWar tools

import java.io.*;
import java.util.*;

abstract class Plugin
{
    public abstract String getName();
    
    public abstract void processArgs(String[] args);
}
