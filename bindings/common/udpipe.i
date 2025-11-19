// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%begin %{
#define SWIG_TYPE_TABLE ufal_udpipe
%}

%include "udpipe_stl.i"
%include "stdint.i"

%{
#include <sstream>
#include "udpipe.h"
using namespace ufal::udpipe;
%}

%template(Children) std::vector<int>;
typedef std::vector<int> Children;

#ifndef HAVE_CUSTOM_BYTES
%template(Bytes) std::vector<uint8_t>;
typedef std::vector<uint8_t> Bytes;
#endif

%template(Comments) std::vector<std::string>;
typedef std::vector<std::string> Comments;

%inline %{
class ProcessingError {
public:
  bool occurred() { return !message.empty(); }
  std::string message;
};
%}


%rename(Token) token;
class token {
 public:
  std::string form;
  std::string misc;

  token(const std::string& form = std::string(), const std::string& misc = std::string());

  // CoNLL-U defined SpaceAfter=No feature
  %rename (getSpaceAfter) get_space_after;
  bool get_space_after() const;
  %rename (setSpaceAfter) set_space_after;
  void set_space_after(bool space_after);

  // UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
  %extend {
    std::string getSpacesBefore() const {
      std::string spaces_before;
      $self->get_spaces_before(spaces_before);
      return spaces_before;
    }

    void setSpacesBefore(const std::string& spaces_before) {
      $self->set_spaces_before(spaces_before);
    }

    std::string getSpacesAfter() const {
      std::string spaces_after;
      $self->get_spaces_after(spaces_after);
      return spaces_after;
    }

    void setSpacesAfter(const std::string& spaces_after) {
      $self->set_spaces_after(spaces_after);
    }

    std::string getSpacesInToken() const {
      std::string spaces_in_token;
      $self->get_spaces_in_token(spaces_in_token);
      return spaces_in_token;
    }

    void setSpacesInToken(const std::string& spaces_in_token) {
      $self->set_spaces_in_token(spaces_in_token);
    }
  }

  // UDPipe-specific TokenRange feature
  %extend {
    bool getTokenRange() const {
      size_t start, end;
      return $self->get_token_range(start, end);
    }

    size_t getTokenRangeStart() const {
      size_t start, end;
      return $self->get_token_range(start, end), start;
    }

    size_t getTokenRangeEnd() const {
      size_t start, end;
      return $self->get_token_range(start, end), end;
    }

    void setTokenRange(size_t start, size_t end) {
      $self->set_token_range(start, end);
    }
  };
};


%rename(Word) word;
class word : public token {
 public:
  // form and misc are inherited from token
  int id;              // 0 is root, >0 is sentence word, <0 is undefined
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  int head;            // head, 0 is root, <0 is undefined
  std::string deprel;  // dependency relation to the head
  std::string deps;    // secondary dependencies

  std::vector<int> children;

  word(int id = -1, const std::string& form = std::string()) : token(form), id(id), head(-1) {}
};
%template(Words) std::vector<word>;
typedef std::vector<word> Words;


%rename(MultiwordToken) multiword_token;
class multiword_token : public token {
 public:
  // form and misc are inherited from token
  %rename(idFirst) id_first;
  %rename(idLast) id_last;
  int id_first, id_last;
  std::string feats; // can contain only Typo=Yes

  multiword_token(int id_first = -1, int id_last = -1, const std::string& form = std::string(), const std::string& misc = std::string())
  : token(form, isc), id_first(id_first), id_last(id_last) {}
};
%template(MultiwordTokens) std::vector<multiword_token>;
typedef std::vector<multiword_token> MultiwordTokens;


%rename(EmptyNode) empty_node;
class empty_node {
 public:
  int id;              // 0 is root, >0 is sentence word, <0 is undefined
  int index;           // index for the current id, should be numbered from 1, 0=undefined
  std::string form;    // form
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  std::string deps;    // secondary dependencies
  std::string misc;    // miscellaneous information

  empty_node(int id = -1, int index = 0) : id(id), index(index) {}
};
%template(EmptyNodes) std::vector<empty_node>;
typedef std::vector<empty_node> EmptyNodes;


%rename(Sentence) sentence;
class sentence {
 public:
  sentence();

  std::vector<word> words;
  %rename(multiwordTokens) multiword_tokens;
  std::vector<multiword_token> multiword_tokens;
  %rename(emptyNodes) empty_nodes;
  std::vector<empty_node> empty_nodes;
  std::vector<std::string> comments;
  %rename(rootForm) root_form;
  static const std::string root_form;

  // Basic sentence modifications
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

  // CoNLL-U defined comments
  %extend {
    bool getNewDoc() const {
      return $self->get_new_doc(nullptr);
    }
    std::string getNewDocId() const {
      std::string id;
      return $self->get_new_doc(&id), id;
    }
    void setNewDoc(bool new_doc, const std::string& id = std::string()) {
      $self->set_new_doc(new_doc, id);
    }

    bool getNewPar() const {
      return $self->get_new_par(nullptr);
    }
    std::string getNewParId() const {
      std::string id;
      return $self->get_new_par(&id), id;
    }
    void setNewPar(bool new_par, const std::string& id = std::string()) {
      $self->set_new_par(new_par, id);
    }

    std::string getSentId() const {
      std::string id;
      return $self->get_sent_id(id), id;
    }
    void setSentId(const std::string& id) {
      $self->set_sent_id(id);
    }

    std::string getText() const {
      std::string text;
      return $self->get_text(text), text;
    }
    void setText(const std::string& id) {
      $self->set_text(id);
    }
  }
};
%template(Sentences) std::vector<sentence>;
typedef std::vector<sentence> Sentences;


%rename(InputFormat) input_format;
%nodefaultctor input_format;
class input_format {
 public:
  virtual ~input_format() {}

  %rename(resetDocument) reset_document;
  virtual void reset_document(const std::string& id = std::string());
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
  static input_format* new_conllu_input_format(const std::string& options = std::string());
  %rename(newGenericTokenizerInputFormat) new_generic_tokenizer_input_format;
  %newobject new_generic_tokenizer_input_format;
  static input_format* new_generic_tokenizer_input_format(const std::string& options = std::string());
  %rename(newHorizontalInputFormat) new_horizontal_input_format;
  %newobject new_horizontal_input_format;
  static input_format* new_horizontal_input_format(const std::string& options = std::string());
  %rename(newVerticalInputFormat) new_vertical_input_format;
  %newobject new_vertical_input_format;
  static input_format* new_vertical_input_format(const std::string& options = std::string());

  %rename(newPresegmentedTokenizer) new_presegmented_tokenizer;
  %newobject new_presegmented_tokenizer;
  static input_format* new_presegmented_tokenizer(input_format* DISOWN);

  static const std::string CONLLU_V1;
  static const std::string CONLLU_V2;
  static const std::string GENERIC_TOKENIZER_NORMALIZED_SPACES;
  static const std::string GENERIC_TOKENIZER_PRESEGMENTED;
  static const std::string GENERIC_TOKENIZER_RANGES;
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
  static output_format* new_conllu_output_format(const std::string& options = std::string());
  %rename(newEpeOutputFormat) new_epe_output_format;
  %newobject new_epe_output_format;
  static output_format* new_epe_output_format(const std::string& options = std::string());
  %rename(newMatxinOutputFormat) new_matxin_output_format;
  %newobject new_matxin_output_format;
  static output_format* new_matxin_output_format(const std::string& options = std::string());
  %rename(newHorizontalOutputFormat) new_horizontal_output_format;
  %newobject new_horizontal_output_format;
  static output_format* new_horizontal_output_format(const std::string& options = std::string());
  %rename(newPlaintextOutputFormat) new_plaintext_output_format;
  %newobject new_plaintext_output_format;
  static output_format* new_plaintext_output_format(const std::string& options = std::string());
  %rename(newVerticalOutputFormat) new_vertical_output_format;
  %newobject new_vertical_output_format;
  static output_format* new_vertical_output_format(const std::string& options = std::string());

  static const std::string CONLLU_V1;
  static const std::string CONLLU_V2;
  static const std::string HORIZONTAL_PARAGRAPHS;
  static const std::string PLAINTEXT_NORMALIZED_SPACES;
  static const std::string VERTICAL_PARAGRAPHS;
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
  static const std::string TOKENIZER_NORMALIZED_SPACES;
  static const std::string TOKENIZER_PRESEGMENTED;
  static const std::string TOKENIZER_RANGES;
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

  %rename(setImmediate) set_immediate;
  void set_immediate(bool immediate);
  %rename(setDocumentId) set_document_id;
  void set_document_id(const std::string& document_id);

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
    %newobject train;
    static std::vector<uint8_t>* train(const std::string& method, const std::vector<sentence>& train, const std::vector<sentence>& heldout,
        const std::string& tokenizer, const std::string& tagger, const std::string& parser, ProcessingError* error = nullptr) {
      std::ostringstream os;
      std::string err;
      if (trainer::train(method, train, heldout, tokenizer, tagger, parser, os, error ? error->message : err)) {
        const std::string& buffer = os.str();
        return new std::vector<uint8_t>(buffer.data(), buffer.data() + buffer.size());
      }
      return new std::vector<uint8_t>();
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
