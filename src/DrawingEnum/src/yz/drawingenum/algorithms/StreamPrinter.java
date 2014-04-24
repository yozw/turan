package yz.drawingenum.algorithms;

import java.io.PrintStream;

public class StreamPrinter {

  private boolean newLine = true;
  private int indent = 0;
  private String indentStr = null;

  private PrintStream out = System.out;

  public StreamPrinter() {
    setIndent(0);
  }

  public StreamPrinter(PrintStream stream) {
    out = stream;
    setIndent(0);
  }

  public void increaseIndent(int delta) {
    setIndent(indent + delta);
  }

  public void setIndent(int indent) {
    this.indent = indent;
    indentStr = null;
  }

  private String getIndentStr() {
    if (indentStr == null) {
      indentStr = "";
      for (int i = 0; i < indent; i++) {
        indentStr += " ";
      }
    }
    return indentStr;
  }

  public void print(Object o) {
    print(o.toString());
  }

  public void println(Object o) {
    println(o.toString());
  }

  public void print(String str) {
    if (newLine) {
      out.print(getIndentStr());
    }
    out.print(str);
    newLine = false;
  }

  public void println(String str) {
    if (newLine) {
      out.print(getIndentStr());
    }
    out.println(str);
    newLine = true;
  }

  public void println() {
    out.println();
    newLine = true;
  }

}
