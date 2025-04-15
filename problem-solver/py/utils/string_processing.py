import json
from natasha import Segmenter, NewsMorphTagger, MorphVocab, Doc, NewsEmbedding

segmenter = Segmenter()
emb = NewsEmbedding()
morph_tagger = NewsMorphTagger(emb)
morph_vocab = MorphVocab()

import sys

def process_text(text):
    doc = Doc(text)
    doc.segment(segmenter)
    doc.tag_morph(morph_tagger)
    
    nouns = []
    for token in doc.tokens:
        if token.pos == 'NOUN':
            token.lemmatize(morph_vocab)
            nouns.append(token.lemma) 
    return nouns

if __name__ == "__main__":
    if len(sys.argv) > 1:
        input_text = sys.argv[1]
        result = process_text(input_text)
        print(json.dumps(result, ensure_ascii=False))

