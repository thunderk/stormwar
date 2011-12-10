// Class to handle Var from StormWar

import java.io.*;
import java.util.*;
import java.lang.*;

// TODO: arrays are not implemented correctly
// TODO: this is not syntax errors friendly

class Var
{
    private String _name;
    private int _type;
    private int _i;
    private float _f;
    private String _s;
    private Vector _va;
    
    public Var()
    {
        setName("");
        setVoid();
    }
    
    public Var(Var v)
    {
        _name = new String(v._name);
        if (v._type == 1)
            setInt(v._i);
        else if (v._type == 2)
            setFloat(v._f);
        else if (v._type == 3)
            setString(v._s);
        else if (v._type == 4)
        {
            setArray();
            for (Enumeration e = v._va.elements(); e.hasMoreElements(); )
            {
                addToArray((Var)e.nextElement());
            }
        }
    }
    
    public Var(File f)
    {
        try
        {
            readFromReader(new BufferedReader(new FileReader(f)));
        }
        catch (Exception e)
        {
            System.out.println("Error while reading file '" + f.getPath() + "':");
            System.out.println(" " + e.getMessage());
        }
    }
    
    public Var(String s)
    {
        try
        {
            readFromReader(new BufferedReader(new StringReader(s)));
        }
        catch (Exception e)
        {
            System.out.println("Error while reading string '" + s + "':");
            System.out.println(" " + e.getMessage());
        }
    }
    
    public String toString(int indent)
    {
        String ret = new String();
        
        if (!_name.equals(""))
        {
            ret += indent(indent) + "#" + _name + " = ";
            if (_type == 4)
                ret += "\n";
        }
        else if (_type != 4)
            ret += indent(indent);
        
        if (_type == 1)
            ret += _i;
        else if (_type == 2)
            ret += _f;
        else if (_type == 3)
            ret += "\"" + _s + "\"";
        else if (_type == 4)
        {
            ret += indent(indent) + "[\n";
            for (Enumeration e = _va.elements(); e.hasMoreElements(); )
            {
                ret += ((Var)e.nextElement()).toString(indent + 1);
                if (e.hasMoreElements())
                    ret += ",\n";
                else
                    ret += "\n";
            }
            ret += indent(indent) + "]";
        }
        
        return ret;
    }
    
    public void setName(String name)
    {
        _name = new String(name);
    }

    public String getName()
    {
        return _name;
    }
    
    public void setVoid()
    {
        _type = 0;
    }
    
    public void setInt(int i)
    {
        _type = 1;
        _i = i;
    }
    
    public void setFloat(float f)
    {
        _type = 2;
        _f = f;
    }
    
    public void setString(String s)
    {
        _type = 3;
        _s = new String(s);
        while (_s.indexOf('\n') != -1)
        {
            _s = _s.substring(0, _s.indexOf('\n')) + "\\n" + _s.substring(_s.indexOf('\n') + 1);
        }
    }

    public String getString()
    {
        return _s;
    }
    
    public void setArray()
    {
        _type = 4;
        _va = new Vector();
    }
    
    public void addToArray(Var v)
    {
        if (_type != 4)
            setArray();
        
        _va.add(new Var(v));
    }
    
    public Enumeration getArrayElements()
    {
        return _va.elements();
    }

    public Var getArrayElement(String s)
    {
        for (ListIterator it = _va.listIterator(); it.hasNext(); )
        {
            Var v = ((Var)it.next());
            if (v._name.equals(s))
                return v;
        }
        return null;
    }
    
    private void readFromReader(Reader s)
    {
        StreamTokenizer tok;
        
        tok = new StreamTokenizer(s);
        tok.eolIsSignificant(false);
        tok.lowerCaseMode(false);
        tok.parseNumbers();
        tok.quoteChar((int)'"');
        tok.wordChars((int)'#', (int)'#');
        tok.wordChars((int)'_', (int)'_');
        try
        {
            tok.nextToken();
        }
        catch (Exception e)
        {
            System.out.println("Unknown IO error for reader " + s);
            System.out.println(" " + e.getMessage());
        }
        readFromTokenizer(tok);
    }
        

    private void readFromTokenizer(StreamTokenizer tok)
    {
        try
        {
            setName("");
            setVoid();

            // name reading
            if (tok.ttype == tok.TT_WORD)
            {
                if (tok.sval.charAt(0) == '#')
                {
                    setName(tok.sval.substring(1));
                    if (tok.nextToken() == '=')
                        tok.nextToken();
                }
            }

            // value reading
            if (tok.ttype == tok.TT_NUMBER)
            {
                // TODO: differenciate floats and integers
                setFloat((float)tok.nval);
                tok.nextToken();
            }
            else if (tok.ttype == '"')
            {
                setString(tok.sval);
                tok.nextToken();
            }
            else if (tok.ttype == '[')
            {
                //read an array
                setArray();
                tok.nextToken();    //skip the '['
                do
                {
                    Var v = new Var();
                    v.readFromTokenizer(tok);
                    if (v._type != 0)
                        addToArray(v);
                    if (tok.ttype == ',')
                        tok.nextToken();
                } while ((tok.ttype != ']') && (tok.ttype != tok.TT_EOF));
                tok.nextToken();
            }
            else if (tok.ttype == '@')
            {
                tok.nextToken();    //skip the '@'
                if (tok.ttype == '"')
                {
                    setString(tok.sval);
                    tok.nextToken();
                }
            }
        }
        catch (Exception e)
        {
            System.out.println("Unknown IO error for tokenizer " + tok);
            System.out.println(" " + e.getMessage());
        }
    }
    
    private String indent(int indent)
    {
        String s = new String();
        for (int i = 0; i < indent; i++)
            s += "    ";
        return s;
    }
}
