Perl UDPipe Bindings
====================

Installation
============

The Perl bindings are available as Ufal-UDPipe package on CPAN.

To compile Perl bindings manually, run make in the bindings/perl directory,
optionally with the options described in UDPipe Installation. Perl 5.10 and
later is supported.

Path to the include headers of the required Perl version must be specified in
the PERL_INCLUDE variable using
  make PERL_INCLUDE=path_to_Perl_includes


Perl Bindings API
=================

UDPipe library bindings is available in the Ufal::UDPipe
(http://search.cpan.org/~straka/Ufal-UDPipe/) package. The classes can be
imported into the current namespace using the :all export tag.

The bindings is a straightforward conversion of the C++ bindings API. Vectors do
not have native Perl interface, see Ufal::UDPipe::Words for reference. Static
methods and enumerations are available only through the module, not through
object instance.


C++ Bindings API
================


Helper Structures
-----------------

  typedef vector<int> Children;

  typedef vector<uint8_t> Bytes;

  typedef vector<string> Comments;

  class ProcessingError {
  public:
    bool occurred();
    string message;
  };

  class Token {
   public:
    string form;
    string misc;

    Token(const string& form = string(), const string& misc = string());

    // CoNLL-U defined SpaceAfter=No feature
    bool getSpaceAfter() const;
    void setSpaceAfter(bool space_after);

    // UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
    string getSpacesBefore() const;
    void setSpacesBefore(const string& spaces_before);
    string getSpacesAfter() const;
    void setSpacesAfter(const string& spaces_after);
    string getSpacesInToken() const;
    void setSpacesInToken(const string& spaces_in_token);

    // UDPipe-specific TokenRange feature
    bool getTokenRange() const;
    size_t getTokenRangeStart() const;
    size_t getTokenRangeEnd() const;
    void setTokenRange(size_t start, size_t end);
  };

  class Word : public Token {
   public:
    // form and misc are inherited from token
    int id;         // 0 is root, >0 is sentence word, <0 is undefined
    string lemma;   // lemma
    string upostag; // universal part-of-speech tag
    string xpostag; // language-specific part-of-speech tag
    string feats;   // list of morphological features
    int head;       // head, 0 is root, <0 is undefined
    string deprel;  // dependency relation to the head
    string deps;    // secondary dependencies

    Children children;

    Word(int id = -1, const string& form = string());
  };
  typedef vector<Word> Words;

  class MultiwordToken : public Token {
   public:
    // form and misc are inherited from token
    int idFirst, idLast;

    MultiwordToken(int id_first = -1, int id_last = -1, const string& form = string(), const string& misc = string());
  };
  typedef vector<MultiwordToken> MultiwordTokens;

  class EmptyNode {
   public:
    int id;          // 0 is root, >0 is sentence word, <0 is undefined
    int index;       // index for the current id, should be numbered from 1, 0=undefined
    string form;     // form
    string lemma;    // lemma
    string upostag;  // universal part-of-speech tag
    string xpostag;  // language-specific part-of-speech tag
    string feats;    // list of morphological features
    string deps;     // secondary dependencies
    string misc;     // miscellaneous information

    EmptyNode(int id = -1, int index = 0) : id(id), index(index) {}
  };
  typedef vector<empty_node> EmptyNodes;

  class Sentence {
   public:
    Sentence();

    Words words;
    MultiwordTokens multiwordTokens;
    EmptyNodes emptyNodes;
    Comments comments;
    static const string rootForm;

    // Basic sentence modifications
    bool empty();
    void clear();
    virtual Word& addWord(const char* form);
    void setHead(int id, int head, const string& deprel);
    void unlinkAllWords();

    // CoNLL-U defined comments
    bool getNewDoc() const;
    string getNewDocId() const;
    void setNewDoc(bool new_doc, const string& id = string());
    bool getNewPar() const;
    string getNewParId() const;
    void setNewPar(bool new_par, const string& id = string());

    string getSentId() const;
    void setSentId(const string& id);
    string getText() const;
    void setText(const string& id);
  };
  typedef vector<Sentence> Sentences;


Main Classes
------------

  class InputFormat {
   public:
    virtual void resetDocument(const string& id = string());
    virtual void setText(const char* text);
    virtual bool nextSentence(Sentence& s, ProcessingError* error = nullptr);

    static InputFormat* newInputFormat(const string& name);
    static InputFormat* newConlluInputFormat(const string& id = string());
    static InputFormat* newGenericTokenizerInputFormat(const string& id = string());
    static InputFormat* newHorizontalInputFormat(const string& id = string());
    static InputFormat* newVerticalInputFormat(const string& id = string());

    static InputFormat* newPresegmentedTokenizer(InputFormat tokenizer);

    static const string CONLLU_V1;
    static const string CONLLU_V2;
    static const string GENERIC_TOKENIZER_NORMALIZED_SPACES;
    static const string GENERIC_TOKENIZER_PRESEGMENTED;
    static const string GENERIC_TOKENIZER_RANGES;
  };

  class OutputFormat {
   public:
    virtual string writeSentence(const Sentence& s);
    virtual string finishDocument();

    static OutputFormat* newOutputFormat(const string& name);
    static OutputFormat* newConlluOutputFormat(const string& options = string());
    static OutputFormat* newEpeOutputFormat(const string& options = string());
    static OutputFormat* newMatxinOutputFormat(const string& options = string());
    static OutputFormat* newHorizontalOutputFormat(const string& options = string());
    static OutputFormat* newPlaintextOutputFormat(const string& options = string());
    static OutputFormat* newVerticalOutputFormat(const string& options = string());

    static const string CONLLU_V1;
    static const string CONLLU_V2;
    static const string HORIZONTAL_PARAGRAPHS;
    static const string PLAINTEXT_NORMALIZED_SPACES;
    static const string VERTICAL_PARAGRAPHS;
  };

  class Model {
   public:
    static Model* load(const char* fname);

    virtual InputFormat* newTokenizer(const string& options) const;
    virtual bool tag(Sentence& s, const string& options, ProcessingError* error = nullptr) const;
    virtual bool parse(Sentence& s, const string& options, ProcessingError* error) const;

    static const string DEFAULT;
    static const string TOKENIZER_PRESEGMENTED;
  };

  class Pipeline {
   public:
    Pipeline(const Model* m, const string& input, const string& tagger, const string& parser, const string& output);

    void setModel(const Model* m);
    void setInput(const string& input);
    void setTagger(const string& tagger);
    void setParser(const string& parser);
    void setOutput(const string& output);

    void setImmediate(bool immediate);
    void setDocumentId(const string& document_id);

    string process(const string& data, ProcessingError* error = nullptr) const;

    static const string DEFAULT;
    static const string NONE;
  };

  class Trainer {
   public:

    static Bytes* train(const string& method, const Sentences& train, const Sentences& heldout,
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
