#!/usr/bin/env python3
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(prog='tokenize')
    parser.add_argument(
        '-m',
        '--model-path',
        help='path to the model file',
        required=True,
    )
    args = parser.parse_args()

    import transformers

    def get_tokenizer(gguf_path: str) -> transformers.PreTrainedTokenizer:
        return transformers.AutoTokenizer.from_pretrained(
            '.',
            gguf_file=gguf_path,
            legacy=False,
        )

    prompt = sys.stdin.read()
    tokenizer = get_tokenizer(args.model_path)
    tokens = tokenizer.encode(prompt)
    print(tokens)

if __name__ == '__main__':
    main()
