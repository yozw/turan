package yz.drawingenum;

import yz.drawingenum.algorithms.*;
import yz.utils.CircularOrder;
import yz.utils.Pair;

import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;

public class DrawingEnum implements Callback {

  Hashtable<Edge, String> edgeColors = new Hashtable<Edge, String>();

  int currentMinCrossings = Integer.MAX_VALUE;
  String minCrossingDrawing = null;

  static PrintStream messageStream = System.err;

  int N = -1;
  int M = -1;

  Vertex[] A, B;

  HashMap<Vertex, CircularOrder<Vertex>> order_constraint = new HashMap<Vertex, CircularOrder<Vertex>>();

  Options options = new Options();

  PrintStream outFile = null;

  public static void main(String[] args) {

    try {
      messageStream.println("Graph Embedding Enumeration (c) 2010-11 Y. Zwols [yz2198@columbia.edu]");

      DrawingEnum mp = new DrawingEnum();
      mp.parseOptions(args);
      mp.run();
    } catch (Exception exception) {
      messageStream.println("Exception: " + exception.getMessage());
      exception.printStackTrace();
    }

  }

  public void parseOptions(String[] args) throws Exception {
    boolean printHelp = false;
    ArrayList<String> regularArguments = new ArrayList<String>();

    for (int i = 0; i < args.length; i++) {
      String arg = args[i];
      if (arg.startsWith("-")) {
        arg = arg.substring(1);
        if (arg.compareToIgnoreCase("es") == 0) {
          options.sanityChecks = true;
        } else if (arg.compareToIgnoreCase("min") == 0) {
          options.calculateMinCrossings = true;
        } else if (arg.compareToIgnoreCase("co") == 0) {
          options.keepCircularOrders = true;
        } else if (arg.compareToIgnoreCase("dot") == 0) {
          options.generateDotFiles = true;
        } else if (arg.compareToIgnoreCase("hist") == 0) {
          options.keepHistory = true;
        } else if (arg.compareToIgnoreCase("h") == 0) {
          printHelp = true;
        } else if (arg.compareToIgnoreCase("-help") == 0) {
          printHelp = true;
        } else if (arg.compareToIgnoreCase("v") == 0) {
          options.outputLevel = 10;
        } else if (arg.compareToIgnoreCase("cl") == 0) {
          options.crossingLimit = Integer.parseInt(args[++i]);
        } else if (arg.compareToIgnoreCase("ct") == 0) {
          options.crossingTarget = Integer.parseInt(args[++i]);
        } else if (arg.compareToIgnoreCase("vector") == 0) {
          options.outputAsVector = true;
        } else {
          throw new Exception("Unrecognized option: " + arg);
        }
      } else {
        regularArguments.add(arg);
      }
    }

    if (regularArguments.size() == 2) {
      N = Integer.parseInt(regularArguments.get(0));
      M = Integer.parseInt(regularArguments.get(1));
    }
    printHelp = printHelp | (N <= 0) | (M <= 0);

    if (printHelp) {
      showHelp();
      System.exit(0);
    }

  }

  public static void showHelp() {
    messageStream.println("Usage: (java) DrawingEnum [options] N M");
    messageStream.println("N and M specify the size of the complete bipartite graph K(N,M)");
    messageStream.println("");
    messageStream.println("Options:");
    messageStream.println("-cl n     crossing limit [default=inf]");
    messageStream.println("-min      determine minimum number of crossings");
    messageStream.println("-ct n     crossing target; program stops when target is reached [default=-1]");
    messageStream.println("-co       keep track of circular orders");
    messageStream.println("-hist     keep track of drawing histories (saved in .dot files)");
    messageStream.println("-dot      generate .dot files");
    messageStream.println("-es       enables sanity checks");
    messageStream.println("-v        verbose mode");
    messageStream.println("-vector   output in vector format instead of human readable format");
  }

  public void run() throws Exception {
    A = new Vertex[N];
    B = new Vertex[M];

    VertexList V = new VertexList();
    EdgeList E = new EdgeList();
    for (int j = 0; j < M; j++) {
      V.add(B[j] = new Vertex("b" + j));
    }
    for (int i = 0; i < N; i++) {
      V.add(A[i] = new Vertex("a" + i));
    }
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        E.add(new Edge(A[i], B[j]));
      }
    }

    String[] colors = {"blue", "red", "forestgreen", "cyan", "black", "purple", "brown", "crimson", "darkseagreen"};
    for (int i = 0; i < E.size(); i++) {
      edgeColors.put(E.get(i), colors[i % colors.length]);
    }

    messageStream.println("Enumerating embeddings of the " + N + "x" + M + " complete bipartite graph.");

    VertexList region0 = new VertexList();
    region0.add(A[0]);
    region0.add(B[0]);
    region0.add(A[1]);
    region0.add(B[1]);

    outFile = System.out; //;new PrintStream("output.txt");

    DrawingEnumeration de = new DrawingEnumeration(V, E);


    DrawingEnumeration.OUTPUT_LEVEL = options.outputLevel;
    DrawingEnumeration.KEEP_CIRCULAR_ORDERS = options.keepCircularOrders;
    DrawingEnumeration.KEEP_HISTORY = options.keepHistory;

    de.setCallback(this);
    de.setSanityChecks(options.sanityChecks);

    if (options.sanityChecks) {
      messageStream.println("Notice: sanity checks are ENABLED");
    }
    assert (reportAssertions());

    de.setOutputLevel(options.outputLevel);

    if (options.outputAsVector) {
      outFile.println(N + " " + M);
    } else {
      outFile.println("graph K " + N + " " + M);
    }

    de.completeDrawing(region0);

    messageStream.println(totalDrawingCount + " embeddings");

    if (options.calculateMinCrossings) {
      if (minCrossingDrawing == null) {
        messageStream.println("The crossing number of this graph is " + currentMinCrossings);
      } else {
        messageStream.println("The crossing number of this graph is " + currentMinCrossings + ", as witnessed by " + minCrossingDrawing);
      }
    }

    outFile.close();
  }

  private static boolean reportAssertions() {
    messageStream.println("Notice: assertions are ENABLED");
    return true;
  }

  int drawingCount = 0;
  int totalDrawingCount = 0;
  long lastUpdateTime = 0;

  public boolean prune(PartialDrawing pd) throws Exception {


    /*		final int nchoose2[] = {0, 0, 1, 3, 6};

    		int anodes = 0, bnodes = 0;
    		for (Node node : pd.getNodes())
    		{
    			String name = node.toString();
    			if (name.startsWith("a"))
    				anodes++;
    			else
    			if (name.startsWith("b"))
    				bnodes++;
    		}
    		int crlowerbound = nchoose2[anodes] * nchoose2[bnodes];
    		if (pd.getCrossingCount() < crlowerbound)
    		{
    			return true;
    		}*/


    /*		for (Entry<Vertex, CircularOrder<Vertex>> entry: order_constraint.entrySet())
        {
    			CircularOrder<Vertex> co = pd.getCircularOrder(entry.getKey());
    			if (co == null)
    				continue;
    			if (!entry.getValue().isSubOrder(co))
    			{
    				//System.out.println("The circular order of " + entry.getKey() + " is " + co + ", " +
    				//		"whereas " + entry.getValue() + " is required. Pruning.");
    				return true;
    			}
    		}*/

    if (options.calculateMinCrossings) {
      if (pd.getCrossingCount() >= currentMinCrossings) {
        return true;
      }
    }

    if (pd.getCrossingCount() > options.crossingLimit) {
      return true;
    }

    return false;
  }

  public boolean drawingComplete(PartialDrawing pd, History history) throws Exception {
    totalDrawingCount++;

    String fname = null;

    boolean compileDot = options.compileDotFiles;
    boolean generateDot = options.generateDotFiles;

    long time = System.currentTimeMillis();

    if (time - lastUpdateTime > 1000) {
      messageStream.print(totalDrawingCount + " embeddings\r");
      lastUpdateTime = time;
    }

    if (options.outputAsVector) {
      outFile.print(totalDrawingCount + " " + pd.getCrossingCount() + " ");

      String str = "";
      if (options.keepCircularOrders) {
        for (int i = 0; i < N; i++) {
          str += A[i] + " " + pd.getCircularOrder(A[i]) + " ";
        }
        for (int i = 0; i < M; i++) {
          str += B[i] + " " + pd.getCircularOrder(B[i]) + " ";
        }
      }

      for (Pair<Edge> crossing : pd.getCrossings()) {
        str += crossing.a + " " + crossing.b + " ";
      }

      outFile.println(str.trim().replace("a", "").replace("b", "").replace("{", "").replace("}", "").replace(",", " "));
    } else {
      String crossings = "";
      for (Pair<Edge> crossing : pd.getCrossings()) {
        crossings += " (" + crossing.a + "," + crossing.b + ")";
      }
      crossings = crossings.trim();

      outFile.print("id " + totalDrawingCount + " ");
      outFile.print("crossing_count " + pd.getCrossingCount() + " ");

      if (options.keepCircularOrders) {
        outFile.print("cyclic_orders ");
        for (int i = 0; i < N; i++) {
          outFile.print(A[i] + " : " + pd.getCircularOrder(A[i]) + " ; ");
        }
        for (int i = 0; i < M; i++) {
          outFile.print(B[i] + " : " + pd.getCircularOrder(B[i]) + " ; ");
        }
      }
      outFile.print("crossings ");
      outFile.println(crossings);
    }

    if (options.generateDotFiles) {
      fname = "drawing" + (++drawingCount);
    }

    if (options.calculateMinCrossings) {
      if (pd.getCrossingCount() < currentMinCrossings) {
        currentMinCrossings = pd.getCrossingCount();
        if (options.generateOptimalDotFile) {
          if (fname == null) {
            fname = "opt";
          }
          generateDot = true;
          compileDot = options.compileOptimalDotFile;
        }
        minCrossingDrawing = fname;
        if (fname != null) {
          messageStream.println("Crossing number <= " + currentMinCrossings + " (witnessed by " + fname + ")");
        } else {
          messageStream.println("Crossing number <= " + currentMinCrossings);
        }
      }
    }

    if (generateDot) {
      writeDotFile(fname + ".dot", pd, history);
      if (compileDot) {
        Runtime.getRuntime().exec("./dotdrawing " + fname);
        messageStream.println("*** Generated and compiled dot file " + fname + ".dot");
      } else {
        messageStream.println("*** Generated dot file " + fname + ".dot");
      }
    }

//		writeGmlFile(fname + ".gml", pd, history);

    return pd.getCrossingCount() > options.crossingTarget;
  }

  public void writeDotFile(String fileName, PartialDrawing pd, History history) throws Exception {
    PrintWriter pw = new PrintWriter(fileName);

    if (history != null) {
      for (String s : history.toArray()) {
        pw.println("# " + s);
      }
    }
    pw.println("graph drawing" + drawingCount + " {");
    for (Node n : pd.getNodes()) {
      if (n.vertex != null) {
        pw.println("  " + n + ";");
      } else {
        pw.println("  " + n + " [height=.2,width=.2,fixedsize=true,shape=box,fontsize=6];");
      }
    }
    for (Arc arc : pd.getArcs()) {
      Edge e = pd.getArcEdge(arc);
      pw.println("  " + arc.u + " -- " + arc.v + " [color=" + edgeColors.get(e) + ", fontsize=6, label=" + e.u + e.v + "];");
    }
    pw.print("{ rank=same; ");
    for (int i = 0; i < N; i++) {
      pw.print("a" + i + " ");
    }
    pw.println("};");
    pw.print("{ rank=same; ");
    for (int i = 0; i < M; i++) {
      pw.print("b" + i + " ");
    }
    pw.println("};");

    pw.println("}");

    pw.close();

  }

  public void writeGmlFile(String fileName, PartialDrawing pd, History history) throws Exception {
    HashMap<Node, Integer> index = new HashMap<Node, Integer>();
    int i = 1;
    for (Node n : pd.getNodes()) {
      index.put(n, i++);
    }

    PrintWriter pw = new PrintWriter(fileName);

    if (history != null) {
      for (String s : history.toArray()) {
        pw.println("# " + s);
      }
    }
    pw.println("graph [");
    pw.println("comment \"" + drawingCount + "\"");
    pw.println("directed 0");
    pw.println("IsPlanar 1");
    for (Node n : pd.getNodes()) {
      pw.println("node [");
      pw.println("id " + index.get(n));
      pw.println("label \"" + n + "\"");
      if (n.vertex != null) {
        pw.println("width 10");
      } else {
        pw.println("width 1");
      }
      pw.println("type \"circle\"");
      pw.println("]");
//" + n + " [height=.2,width=.2,fixedsize=true,shape=box,fontsize=6];");
    }
    for (Arc arc : pd.getArcs()) {
      Edge e = pd.getArcEdge(arc);
      pw.println("edge [");
      pw.println("source " + index.get(arc.u));
      pw.println("target " + index.get(arc.v));
      pw.println("]");
// [color=" + edgeColors.get(e) + ", fontsize=6, label=" + e.u + e.v + "];");
    }

    pw.println("]");

    pw.close();

  }

}
