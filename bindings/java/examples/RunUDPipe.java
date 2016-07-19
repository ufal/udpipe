// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

import cz.cuni.mff.ufal.udpipe.*;
import java.util.Scanner;

class RunUDPipe {
  public static void main(String[] args) {
    if (args.length < 3) {
      System.err.println("Usage: RunUDPipe input_format output_format model_file");
      System.exit(1);
    }

    System.err.print("Loading model: ");
    Model model = Model.load(args[2]);
    if (model == null) {
      System.err.println("Cannot load model from file '" + args[2] + "'");
      System.exit(1);
    }
    System.err.println("done");


    Pipeline pipeline = new Pipeline(model, args[0], Pipeline.getDEFAULT(), Pipeline.getDEFAULT(), args[1]);
    ProcessingError error = new ProcessingError();
    Scanner reader = new Scanner(System.in);

    // Read while input
    StringBuilder textBuilder = new StringBuilder();
    while (reader.hasNextLine()) {
      textBuilder.append(reader.nextLine());
      textBuilder.append('\n');
    }

    // Tokenize and tag
    String text = textBuilder.toString();
    String processed = pipeline.process(text, error);

    if (error.occurred()) {
      System.err.println("Cannot read input CoNLL-U: " + error.getMessage());
      System.exit(1);
    }
    System.out.print(processed);
  }
}
