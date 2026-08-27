

















import os
import re
import requests
from bs4 import BeautifulSoup











ARTICLES = [
    (
        "Artificial Intelligence",
        "https://en.wikipedia.org/wiki/Artificial_intelligence"
    ),
    (
        "Machine Learning",
        "https://en.wikipedia.org/wiki/Machine_learning"
    ),
    (
        "Quantum Computing",
        "https://en.wikipedia.org/wiki/Quantum_computing"
    ),
    (
        "Blockchain Technology",
        "https://en.wikipedia.org/wiki/Blockchain"
    ),
    (
        "CRISPR Gene Editing",
        "https://en.wikipedia.org/wiki/CRISPR_gene_editing"
    ),
    (
        "Space Exploration",
        "https://en.wikipedia.org/wiki/Space_exploration"
    ),
    (
        "Renewable Energy",
        "https://en.wikipedia.org/wiki/Renewable_energy"
    ),
    (
        "Cybersecurity",
        "https://en.wikipedia.org/wiki/Computer_security"
    ),
    (
        "Cloud Computing",
        "https://en.wikipedia.org/wiki/Cloud_computing"
    ),
    (
        "Robotics",
        "https://en.wikipedia.org/wiki/Robotics"
    ),
    (
        "Evolution of DNA",
        "https://en.wikipedia.org/wiki/DNA"
    ),
]


def clean_text(raw_text):
    """
    Cleans extracted text by removing:
      - Wikipedia citation brackets like [1], [23], [citation needed]
      - Excessive whitespace and blank lines
      - Non-breaking spaces and other Unicode artifacts

    Args:
        raw_text (str): The raw text extracted from HTML.

    Returns:
        str: Cleaned, readable text.
    """
    
    text = re.sub(r'\[\d+\]', '', raw_text)
    text = re.sub(r'\[citation needed\]', '', text, flags=re.IGNORECASE)
    text = re.sub(r'\[.*?\]', '', text)

    
    text = text.replace('\xa0', ' ')

    
    text = re.sub(r' +', ' ', text)

    
    text = re.sub(r'\n\s*\n', '\n\n', text)

    
    lines = [line.strip() for line in text.split('\n')]
    text = '\n'.join(lines)

    
    text = text.strip()

    return text


def scrape_wikipedia_article(url):
    """
    Scrapes the main text content from a Wikipedia article.

    Strategy:
      1. Fetch the page HTML using requests.get().
      2. Parse with BeautifulSoup using the built-in 'html.parser'.
      3. Find the main content div (id='mw-content-text').
      4. Extract all <p> (paragraph) tags from the content area.
      5. Join and clean the extracted text.

    Args:
        url (str): Full URL of the Wikipedia article.

    Returns:
        str: Cleaned article text, or an empty string on failure.
    """
    
    headers = {
        'User-Agent': 'CustomSearchEngine/1.0 (Educational Project)'
    }

    try:
        
        response = requests.get(url, headers=headers, timeout=15)
        response.raise_for_status()  

    except requests.RequestException as e:
        print(f"  ERROR: Failed to fetch {url}")
        print(f"         {e}")
        return ""

    
    soup = BeautifulSoup(response.text, 'html.parser')

    
    
    content_div = soup.find('div', {'id': 'mw-content-text'})

    if content_div is None:
        print(f"  WARNING: Could not find content div for {url}")
        return ""

    
    
    paragraphs = content_div.find_all('p')

    
    raw_text = '\n'.join(p.get_text() for p in paragraphs)

    
    cleaned = clean_text(raw_text)

    return cleaned


def save_document(filepath, title, content):
    """
    Saves a document to disk in the format expected by the C++ engine.

    File format:
      Line 1:    Document title (used by C++ as Document.title)
      Lines 2+:  Document content (used by C++ as Document.content)

    Args:
        filepath (str): Output file path.
        title (str):    Document title.
        content (str):  Document text content.
    """
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(title + '\n')
        f.write(content)


def main():
    """
    Main entry point. Scrapes all target articles and saves to corpus/.
    """
    
    
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)  
    corpus_dir = os.path.join(project_root, 'corpus')

    
    os.makedirs(corpus_dir, exist_ok=True)

    print("=" * 65)
    print("  Custom Search Engine — Web Scraper")
    print("  Scraping Wikipedia articles for the C++ search engine corpus")
    print("=" * 65)
    print()

    success_count = 0
    total_chars = 0

    for i, (title, url) in enumerate(ARTICLES):
        
        
        safe_name = title.lower().replace(' ', '_')
        safe_name = re.sub(r'[^a-z0-9_]', '', safe_name)
        filename = f"doc_{i:02d}_{safe_name}.txt"
        filepath = os.path.join(corpus_dir, filename)

        print(f"  [{i + 1}/{len(ARTICLES)}] Scraping: {title}")
        print(f"         URL: {url}")

        
        content = scrape_wikipedia_article(url)

        if content:
            
            save_document(filepath, title, content)
            char_count = len(content)
            total_chars += char_count
            success_count += 1
            print(f"         Saved: {filename} ({char_count:,} chars)")
        else:
            print(f"         SKIPPED (no content extracted)")

        print()

    
    print("=" * 65)
    print(f"  Scraping complete!")
    print(f"  Documents saved:  {success_count}/{len(ARTICLES)}")
    print(f"  Total characters: {total_chars:,}")
    print(f"  Output directory: {corpus_dir}")
    print("=" * 65)
    print()
    print("  Next step: compile and run the C++ search engine:")
    print("    g++ -std=c++17 -O2 -Wall -Wextra -I include src/*.cpp -o search_engine.exe")
    print("    .\\search_engine.exe")
    print()


if __name__ == '__main__':
    main()
