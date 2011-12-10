// i18n updater

import java.io.*;
import java.util.*;

class UpdateI18N extends Plugin
{
    public UpdateI18N()
    {

    }



    public void processArgs(String[] args)
    {
        // check args
        if (args.length < 2)
        {
            System.out.println("Usage of tool 'update18n':");
            System.out.println(" {updatei18n i18n_file input1 [input2...]}  where:");
            System.out.println("   i18n_file is the original translation file to read (without any flag).");
            System.out.println("   input<n> are files or directories to process in input.");
            return;
        }

        File forig = new File(args[0]);
        if ((!forig.exists()) || (!forig.isFile()) || (!forig.canRead()))
        {
            System.out.println("Input file '" + forig.getPath() + "' not readable.");
            return;
        }
        Var v = new Var(forig);

        // read translations
        //System.out.println("Reading original translations from '" + forig.getPath() + "'...");
        Hashtable origtrans = new Hashtable();
        for (Enumeration e = v.getArrayElements(); e.hasMoreElements(); )
        {
            Var vt = (Var)e.nextElement();
            origtrans.put(vt.getArrayElement("o").getString(), vt.getArrayElement("t").getString());
        }
        //System.out.println(origtrans);

        // process input files
        Hashtable unused = new Hashtable();
        for (Enumeration e = origtrans.keys(); e.hasMoreElements(); )
        {
            String s = (String)e.nextElement();
            unused.put(s, origtrans.get(s));
        }
        Hashtable missing = new Hashtable();
        try
        {
            for (int i = 1; i < args.length; i++)
            {
                processInput(origtrans, unused, missing, new File(args[i]));
            }
        }
        catch (Exception e)
        {
             System.out.println("Error while processing input files :");
             System.out.println(" " + e.getMessage());
             return;
        }

        //System.out.println("Missing : " + missing);
        //System.out.println("Unused : " + unused);

        // create new translation Var
        Var vtn = new Var();
        vtn.setName(v.getName());
        vtn.setArray();

        for (Enumeration e = origtrans.keys(); e.hasMoreElements(); )
        {
            Var vo = new Var();
            Var vn = new Var();
            String s = (String)e.nextElement();

            vn.setArray();

            vo.setName("o");
            vo.setString(s);
            vn.addToArray(vo);

            vo.setName("t");
            vo.setString((String)origtrans.get(s));
            vn.addToArray(vo);

            if (unused.containsKey(s))
            {
                vo.setName("flag");
                vo.setString("UNUSED");
                vn.addToArray(vo);
            }

            vtn.addToArray(vn);
        }
        for (Enumeration e = missing.keys(); e.hasMoreElements(); )
        {
            Var vo = new Var();
            Var vn = new Var();
            String s = (String)e.nextElement();

            vn.setArray();

            vo.setName("o");
            vo.setString(s);
            vn.addToArray(vo);

            vo.setName("t");
            String s2 = fuzzy(s, origtrans);
            if (s2.equals(""))
            {
                vo.setString(s);
            }
            else
            {
                vo.setString(s2);
            }
            vn.addToArray(vo);

            if (s2.equals(""))
            {
                vo.setName("flag");
                vo.setString("MISSING");
                vn.addToArray(vo);
            }
            else
            {
                vo.setName("flag");
                vo.setString("GUESSED");
                vn.addToArray(vo);
            }

            vtn.addToArray(vn);
        }
        System.out.println(vtn.toString(0));
    }



    public String getName()
    {
        return "updatei18n";
    }



    private void processInput(Hashtable origtrans, Hashtable unused, Hashtable missing, File f) throws Exception
    {
        //System.out.println("Processing '" + f.getPath() + "'...");
        if (!f.exists())
            throw new Exception("File '" + f.getPath() + "' doesn't exist.");

        if (f.isDirectory())
        {
            //recursive processing
            File[] files = f.listFiles();
            for (int i = 0; i < files.length; i++)
            {
                processInput(origtrans, unused, missing, files[i]);
            }
        }
        else if (f.isFile())
        {
            if (!f.canRead())
                throw new Exception("Can't read file '" + f.getPath() + "'.");

            FileReader r = new FileReader(f);
            String s;

            //test if the file seems binary
            int i = 0, cnt = 0;
            while (i < 30 && r.ready())
            {
                char c = (char)r.read();
                i++;
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                    c == '#' || c == '_' || c == '(' || c == ')' || c == '[' || c == ']' ||
                    c == '{' || c == '}' || c == '.' || c == '-' || c == '@' || c == '=' ||
                    c == '+' || c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '$' ||
                    c == '\"' || c == '\'' || c == '~' || c == '&' || c == '|' || c == '*')
                {
                    cnt++;
                }
            }
            if (cnt < i / 2)
            {
                //binary file, ignoring
                System.out.println("Ignored binary file '" + f.getPath() + "'.");
                return;
            }

            //process
            r = new FileReader(f);
            while (r.ready())
            {
                int c = r.read();
                s = null;
                if ((c == '_') && (r.read() == '(') && (r.read() == '"'))
                {
                    s = new String();
                    while (r.ready() && ((c = r.read()) != '"'))
                    {
                        s += (char)c;
                    }
                }
                if ((c == '"') && (r.read() == '&'))
                {
                    s = new String();
                    while (r.ready() && ((c = r.read()) != '"'))
                    {
                        s += (char)c;
                    }
                }

                if (s != null)
                {
                    //System.out.println("Found i18n string : " + s);

                    if (origtrans.containsKey(s))
                    {
                        //string is in origtrans, deleting it from unused
                        if (unused.containsKey(s))
                        {
                            unused.remove(s);
                        }
                    }
                    else
                    {
                        //string isn't present in origtrans, adding it to missing
                        if (!missing.containsKey(s))
                        {
                            missing.put(s, new String(""));
                        }
                    }
                }
            }
        }
        else
        {
            throw new Exception("Unknown file type for '" + f.getPath() + "'.");
        }
    }


    private double fuzzy_score(String orig, String match)
    {
        //System.out.println(orig);
        //System.out.println(match);
        orig = orig.toLowerCase();
        match = match.toLowerCase();
        //System.out.println( "match = " + StringSimilarity.compareStrings(orig, match));
        return StringSimilarity.compareStrings(orig, match);
    }

    private String fuzzy(String orig, Hashtable origtrans)
    {
        //fuzzy proposals
        String ret = null;
        double max = 0.0, sc;
        for (Enumeration e = origtrans.keys(); e.hasMoreElements(); )
        {
            String s = (String)e.nextElement();

            sc = fuzzy_score(orig, s);
            if (sc > max)
            {
                ret = (String)origtrans.get(s);
                max = sc;
            }
        }

        if (max > 0.5)
        {
            return ret;
        }
        else
        {
            return "";
        }
    }
}
