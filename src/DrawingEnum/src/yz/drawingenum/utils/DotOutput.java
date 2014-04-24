package yz.drawingenum.utils;

import yz.drawingenum.algorithms.*;

import java.io.PrintWriter;
import java.util.HashMap;

public class DotOutput {

  static final String[] colors = {"blue", "red", "forestgreen", "cyan", "black", "purple", "brown", "crimson", "darkseagreen",
      "darkslategray", "chocolate", "goldenrod", "deeppink", "coral", "gray50", "darkorchid4", "dodgerblue4", "dodgerblue", "salmon",
      "yellowgreen"
  };

  public static void writeDotFile(String fileName, PartialDrawing pd) throws Exception {
    writeDotFile(fileName, pd, null);
  }

  public static void writeDotFile(String fileName, PartialDrawing pd, History history) throws Exception {
    PrintWriter pw = new PrintWriter(fileName);
    writeDotFile(pw, pd, history);
    pw.close();
  }

  public static void writeDotFile(PrintWriter pw, PartialDrawing pd) throws Exception {
    writeDotFile(pw, pd, null);
  }

  public static void writeDotFile(PrintWriter pw, PartialDrawing pd, History history) throws Exception {

    HashMap<Edge, String> edgeColors = new HashMap<Edge, String>();
    int colorIndex = 0;

    if (history != null) {
      for (String s : history.toArray()) {
        pw.println("# " + s);
      }
    }

    pw.println("#");
    pw.println("# The regions are: ");
    for (int i = 0; i < pd.regionCount(); i++) {
      pw.println("# " + pd.region(i));
    }
    pw.println();

    pw.println("graph drawing {");
    for (Node n : pd.getNodes()) {
      if (n.vertex != null) {
        pw.println("  " + n + ";");
      } else {
        pw.println("  " + n + " [height=.2,width=.2,fixedsize=true,shape=box,fontsize=6];");
      }
    }
    for (Arc arc : pd.getArcs()) {
      Edge e = pd.getArcEdge(arc);
      String color;
      if (edgeColors.containsKey(e)) {
        color = edgeColors.get(e);
      } else {
        colorIndex = (colorIndex + 1) % colors.length;
        color = colors[colorIndex];
        edgeColors.put(e, color);
      }

      pw.println("  " + arc.u + " -- " + arc.v + " [color=" + color + ", fontsize=6, label=" + e.u + e.v + "];");
    }

    /*
     * pw.print("{ rank=same; "); for (int i = 0; i < N; i++) pw.print("a" +
     * i + " "); pw.println("};"); pw.print("{ rank=same; "); for (int i =
     * 0; i < M; i++) pw.print("b" + i + " ");
     */
    pw.println("};");
  }
}
