
import os
from groq import Groq
from dotenv import load_dotenv

from pathlib import Path
load_dotenv(Path(__file__).parent / "GROQ_API_KEY.env")

client = Groq(
    api_key=os.getenv("GROQ_API_KEY")
)

def identify_key(user_input):

    response = client.chat.completions.create(
        model="openai/gpt-oss-20b",

        messages=[
            {
                "role": "system",
                "content": """
                You are a simple key selection assistant.

                The user wants to collect one of three keys.

                Your job is to interpret their response and
                identify which key they want.

                Key 1 = First key
                Key 2 = Second key
                Key 3 = Third key

                Understand natural language, including:
                - Numbers (one, two, three)
                - Ordinals (first, second, third)
                - Indirect requests
                - Casual language

                Return ONLY one integer:
                1, 2, 3 or 0.

                Return 0 if the response is unclear,
                unrelated or does not identify a key.
                """
            },
            {
                "role": "user",
                "content": user_input
            }
        ],

        temperature=0
    )

    result = response.choices[0].message.content.strip()

    try:
        key_id = int(result)

        if key_id in [1, 2, 3]:
            return key_id

        return 0

    except ValueError:
        return 0

def interpret_yes_no(user_input):
    """Returns 'yes', 'no' or 'unclear'."""

    # Fast path: skip the API call for obvious answers
    quick = user_input.lower().strip()
    if quick in ("yes", "y", "no", "n"):
        return "yes" if quick.startswith("y") else "no"

    try:
        response = client.chat.completions.create(
            model="openai/gpt-oss-20b",
            messages=[
                {
                    "role": "system",
                    "content": """
                    You are classifying a reply to a yes/no question.

                    Decide whether the user agrees (yes), declines (no),
                    or is unclear.

                    Treat as YES: yes, yeah, yep, sure, ok, go ahead,
                    please do, I would like that, I'd like you to,
                    sounds good, do it, and similar.

                    Treat as NO: no, nope, nah, cancel, stop, not now,
                    I don't want that, never mind, and similar.

                    Return ONLY one word: yes, no or unclear.
                    Return unclear if the reply is unrelated, ambiguous,
                    or does not answer the question.
                    """
                },
                {"role": "user", "content": user_input}
            ],
            temperature=0
        )

        result = response.choices[0].message.content.strip().lower()
        return result if result in ("yes", "no") else "unclear"

    except Exception as e:
        print(f"(AI error: {e})")
        return "unclear"