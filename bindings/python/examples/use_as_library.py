# -*- coding: utf-8 -*-
from __future__ import print_function
from ufal.udpipe import Model, Pipeline, ProcessingError

MODELS = {
    "swe": "../udpipe-ud-1.2-160523/swedish-ud-1.2-160523.udpipe"
}


class Tagger:
    def __init__(self, language):
        model_path = MODELS.get(language, None)
        model = Model.load(model_path)
        if not model:
            raise Exception("Cannot load model from file '%s'\n" % model_path)

        self.model = model

    def tag(self, text):
        pipeline = Pipeline(
            self.model,
            "tokenize",
            Pipeline.DEFAULT,
            Pipeline.DEFAULT,
            "conllu"
        )
        error = ProcessingError()

        processed = pipeline.process(text, error)
        if error.occurred():
            raise Exception(error.message)

        return processed

if __name__ == '__main__':
    tagger = Tagger(language="swe")
    print(tagger.tag(u"Fördomen har alltid sin rot i vardagslivet."))
