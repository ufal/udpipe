// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

using Ufal.UDPipe;
using System;
using System.Text;

class RunUDPipe {
    public static int Main(string[] args) {
        if (args.Length < 3) {
            Console.Error.WriteLine("Usage: RunUDPipe input_format(tokenize|conllu|horizontal|vertical) output_format(conllu) model");
            return 1;
        }

        Console.Error.Write("Loading model: ");
        Model model = Model.load(args[2]);
        if (model == null) {
            Console.Error.WriteLine("Cannot load model from file '{0}'", args[2]);
            return 1;
        }
        Console.Error.WriteLine("done");

        Pipeline pipeline = new Pipeline(model, args[0], Pipeline.DEFAULT, Pipeline.DEFAULT, args[1]);
        ProcessingError error = new ProcessingError();

        // Read whole input
        StringBuilder textBuilder = new StringBuilder();
        string line;
        while (!string.IsNullOrEmpty(line = Console.In.ReadLine()))
            textBuilder.AppendLine(line);

        // Process data
        string text = textBuilder.ToString();
        string processed = pipeline.process(text, error);

        if (error.occurred()) {
            Console.Error.WriteLine("An error occurred in RunUDPipe: {0}", error.message);
            return 1;
        }
        Console.Write(processed);

        return 0;
    }
}
