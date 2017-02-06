// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%include "udpipe_stl.i"

%{
#include <sstream>
#include "udpipe.h"
using namespace ufal::udpipe;
%}

%template(Children) std::vector<int>;
typedef std::vector<int> Children;

%template(Comments) std::vector<std::string>;
typedef std::vector<std::string> Comments;

%inline %{
class ProcessingError {
public:
  bool occurred() { return !message.empty(); }
  std::string message;
};
%}


%rename(Word) word;
class word {
 public:
  int id;              // 0 is root, >0 is sentence word, <0 is undefined
  std::string form;    // form
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  int head;            // head, 0 is root, <0 is undefined
  std::string deprel;  // dependency relation to the head
  std::string deps;    // secondary dependencies
  std::string misc;    // miscellaneous information

  std::vector<int> children;

  word(int id = -1, const std::string& form = std::string()) : id(id), form(form), head(-1) {}
};
%template(Words) std::vector<word>;
typedef std::vector<word> Words;


%rename(MultiwordToken) multiword_token;
class multiword_token {
 public:
  %rename(idFirst) id_first;
  %rename(idLast) id_last;
  int id_first, id_last;
  std::string form;
  std::string misc;

  multiword_token(int id_first = -1, int id_last = -1, const std::string& form = std::string(), const std::string& misc = std::string())
  : id_first(id_first), id_last(id_last), form(form), misc(misc) {}
};
%template(MultiwordTokens) std::vector<multiword_token>;
typedef std::vector<multiword_token> MultiwordTokens;


%rename(Sentence) sentence;
class sentence {
 public:
  sentence();

  std::vector<word> words;
  %rename(multiwordTokens) multiword_tokens;
  std::vector<multiword_token> multiword_tokens;
  std::vector<std::string> comments;
  %rename(rootForm) root_form;
  static const std::string root_form;

  bool empty();
  void clear();
  %extend {
    %rename(addWord) add_word;
    virtual word& add_word(const char* form) {
      return $self->add_word(form);
    }
  }
  %rename(setHead) set_head;
  void set_head(int id, int head, const std::string& deprel);
  %rename(unlinkAllNodes) unlink_all_words;
  void unlink_all_words();
};
%template(Sentences) std::vector<sentence>;
typedef std::vector<sentence> Sentences;


%rename(InputFormat) input_format;
%nodefaultctor input_format;
class input_format {
 public:
  virtual ~input_format() {}

  %rename(resetDocument) reset_document;
  virtual void reset_document();
  %extend {
    %rename(setText) set_text;
    virtual void set_text(const char* text) {
      $self->set_text(text, true);
    }
  }
  %extend {
    %rename(nextSentence) next_sentence;
    virtual bool next_sentence(sentence& s, ProcessingError* error = nullptr) {
      std::string err;
      return $self->next_sentence(s, error ? error->message : err);
    }
  }

  %rename(newInputFormat) new_input_format;
  %newobject new_input_format;
  static input_format* new_input_format(const std::string& name);
  %rename(newConlluInputFormat) new_conllu_input_format;
  %newobject new_conllu_input_format;
  static input_format* new_conllu_input_format();
  %rename(newHorizontalInputFormat) new_horizontal_input_format;
  %newobject new_horizontal_input_format;
  static input_format* new_horizontal_input_format();
  %rename(newVerticalInputFormat) new_vertical_input_format;
  %newobject new_vertical_input_format;
  static input_format* new_vertical_input_format();

  %rename(newPresegmentedTokenizer) new_presegmented_tokenizer;
  %newobject new_presegmented_tokenizer;
  static input_format* new_presegmented_tokenizer(input_format* DISOWN);
};


%rename(OutputFormat) output_format;
%nodefaultctor output_format;
class output_format {
 public:
  virtual ~output_format() {}

  %extend {
    %rename(writeSentence) write_sentence;
    virtual std::string write_sentence(const sentence& s) {
      std::ostringstream ostr;
      $self->write_sentence(s, ostr);
      return ostr.str();
    }

    %rename(finishDocument) finish_document;
    virtual std::string finish_document() {
      std::ostringstream ostr;
      $self->finish_document(ostr);
      return ostr.str();
    }
  }

  %rename(newOutputFormat) new_output_format;
  %newobject new_output_format;
  static output_format* new_output_format(const std::string& name);
  %rename(newConlluOutputFormat) new_conllu_output_format;
  %newobject new_conllu_output_format;
  static output_format* new_conllu_output_format();
  %rename(newMatxinOutputFormat) new_matxin_output_format;
  %newobject new_matxin_output_format;
  static output_format* new_matxin_output_format();
  %rename(newHorizontalOutputFormat) new_horizontal_output_format;
  %newobject new_horizontal_output_format;
  static output_format* new_horizontal_output_format();
  %rename(newVerticalOutputFormat) new_vertical_output_format;
  %newobject new_vertical_output_format;
  static output_format* new_vertical_output_format();
};


%rename(Model) model;
%nodefaultctor model;
class model {
 public:
  virtual ~model() {}

  %newobject load;
  static model* load(const char* fname);

  %rename(newTokenizer) new_tokenizer;
  %newobject new_tokenizer;
  virtual input_format* new_tokenizer(const std::string& options) const = 0;
  %extend {
    virtual bool tag(sentence& s, const std::string& options, ProcessingError* error = nullptr) const {
      std::string str;

      return $self->tag(s, options, error ? error->message : str);
    }
  }
  %extend {
    virtual bool parse(sentence& s, const std::string& options, ProcessingError* error = nullptr) const {
      std::string str;

      return $self->parse(s, options, error ? error->message : str);
    }
  }

  static const std::string DEFAULT;
  static const std::string TOKENIZER_PRESEGMENTED;
};


%rename(Pipeline) pipeline;
class pipeline {
 public:
  pipeline(const model* m, const std::string& input, const std::string& tagger, const std::string& parser, const std::string& output);

  %rename(setModel) set_model;
  void set_model(const model* m);
  %rename(setInput) set_input;
  void set_input(const std::string& input);
  %rename(setTagger) set_tagger;
  void set_tagger(const std::string& tagger);
  %rename(setParser) set_parser;
  void set_parser(const std::string& parser);
  %rename(setOutput) set_output;
  void set_output(const std::string& output);

  %extend {
    std::string process(const std::string& data, ProcessingError* error = nullptr) const {
      std::istringstream is(data);
      std::ostringstream os;
      std::string err;

      return $self->process(is, os, error ? error->message : err) ? os.str() : std::string();
    }
  }

  static const std::string DEFAULT;
  static const std::string NONE;
};


%rename(Trainer) trainer;
class trainer {
 public:

  %extend {
    static std::string train(const std::string& method, const std::vector<sentence>& train, const std::vector<sentence>& heldout,
                             const std::string& tokenizer, const std::string& tagger, const std::string& parser,
                             ProcessingError* error = nullptr) {
      std::ostringstream os;
      std::string err;

      return trainer::train(method, train, heldout, tokenizer, tagger, parser, os, error ? error->message : err) ? os.str() : std::string();
    }
  }

  static const std::string DEFAULT;
  static const std::string NONE;
};


%rename(Evaluator) evaluator;
class evaluator {
 public:
  evaluator(const model* m, const std::string& tokenizer, const std::string& tagger, const std::string& parser);

  %rename(setModel) set_model;
  void set_model(const model* m);
  %rename(setTokenizer) set_tokenizer;
  void set_tokenizer(const std::string& tokenizer);
  %rename(setTagger) set_tagger;
  void set_tagger(const std::string& tagger);
  %rename(setParser) set_parser;
  void set_parser(const std::string& parser);

  %extend {
    std::string evaluate(const std::string& data, ProcessingError* error = nullptr) const {
      std::istringstream is(data);
      std::ostringstream os;
      std::string err;

      return $self->evaluate(is, os, error ? error->message : err) ? os.str() : std::string();
    }
  }

  static const std::string DEFAULT;
  static const std::string NONE;
};


%rename(Version) version;
class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;
  std::string prerelease;

  // Returns current version.
  static version current();
};
