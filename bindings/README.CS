C# UDPipe Bindings
==================

Installation
============

Binary C# bindings are available in UDPipe binary packages.

To compile C# bindings manually, run make in the bindings/csharp directory,
optionally with the options described in UDPipe Installation.


C# Bindings API
===============

UDPipe library bindings is available in the Ufal.UDPipe namespace.

The bindings is a straightforward conversion of the C++ bindings API. The
bindings requires native C++ library libudpipe_csharp (called udpipe_csharp on
Windows).


C++ Bindings API
================


Helper Structures
-----------------

  typedef vector<int> Children;

  typedef vector<string> Comments;

  class ProcessingError {
  public:
    bool occurred();
    string message;
  };

  class Word {
   public:
    int id;         // 0 is root, >0 is sentence word, <0 is undefined
    string form;    // form
    string lemma;   // lemma
    string upostag; // universal part-of-speech tag
    string xpostag; // language-specific part-of-speech tag
    string feats;   // list of morphological features
    int head;       // head, 0 is root, <0 is undefined
    string deprel;  // dependency relation to the head
    string deps;    // secondary dependencies
    string misc;    // miscellaneous information

    Children children;

    Word(int id = -1, const string& form = string());
  };
  typedef vector<Word> Words;

  class MultiwordToken {
   public:
    int idFirst, idLast;
    string form;
    string misc;

    MultiwordToken(int id_first = -1, int id_last = -1, const string& form = string(), const string& misc = string());
  };
  typedef vector<MultiwordToken> MultiwordTokens;

  class Sentence {
   public:
    Sentence();

    Words words;
    MultiwordTokens multiwordTokens;
    Comments comments
    static const string rootForm;

    bool empty();
    void clear();
    virtual Word& addWord(const char* form);
    void setHead(int id, int head, const string& deprel);
    void unlinkAllWords();
  };
  typedef vector<Sentence> Sentences;


Main Classes
------------

  class InputFormat {
   public:
    virtual void setText(const char* text);
    virtual bool nextSentence(Sentence& s, ProcessingError* error = nullptr);

    static InputFormat* newInputFormat(const string& name);
    static InputFormat* newConlluInputFormat();
    static InputFormat* newHorizontalInputFormat();
    static InputFormat* newVerticalInputFormat();
  };

  class OutputFormat {
   public:
    virtual string writeSentence(const Sentence& s);
    virtual string finishDocument();

    static OutputFormat* newOutputFormat(const string& name);
    static OutputFormat* newConlluOutputFormat();
    static OutputFormat* newMatxinOutputFormat();
    static OutputFormat* newHorizontalOutputFormat();
    static OutputFormat* newVerticalOutputFormat();
  };

  class Model {
   public:
    static Model* load(const char* fname);

    virtual InputFormat* newTokenizer(const string& options) const;
    virtual bool tag(Sentence& s, const string& options, ProcessingError* error = nullptr) const;
    virtual bool parse(Sentence& s, const string& options, ProcessingError* error) const;

    static const string DEFAULT;
  };

  class Pipeline {
   public:
    Pipeline(const Model* m, const string& input, const string& tagger, const string& parser, const string& output);

    void setModel(const Model* m);
    void setInput(const string& input);
    void setTagger(const string& tagger);
    void setParser(const string& parser);
    void setOutput(const string& output);

    string process(const string& data, ProcessingError* error = nullptr) const;

    static const string DEFAULT;
    static const string NONE;
  };

  class Trainer {
   public:

    static string train(const string& method, const Sentences& train, const Sentences& heldout,
                        const string& tokenizer, const string& tagger, const string& parser,
                        ProcessingError* error = nullptr);

    static const string DEFAULT;
    static const string NONE;
  };

  class Evaluator {
   public:
    Evaluator(const Model* m, const string& tokenizer, const string& tagger, const string& parser);

    void setModel(const Model* m);
    void setTokenizer(const string& tokenizer);
    void setTagger(const string& tagger);
    void setParser(const string& parser);

    string evaluate(const string& data, ProcessingError* error = nullptr) const;

    static const string DEFAULT;
    static const string NONE;
  };

  class Version {
   public:
    unsigned major;
    unsigned minor;
    unsigned patch;
    string prerelease;

    // Returns current version.
    static version current();
  };
