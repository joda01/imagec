import requests

def get_github_releases(owner, repo):
    url = f"https://api.github.com/repos/{owner}/{repo}/releases"
    headers = {"Accept": "application/vnd.github.v3+json"}
    releases = []
    page = 1
    
    while True:
        response = requests.get(url, headers=headers, params={"page": page, "per_page": 100})
        
        if response.status_code != 200:
            print(f"Failed to fetch releases: {response.status_code}")
            return []
        
        data = response.json()
        if not data:
            break
        
        releases.extend([(release["tag_name"], release.get("body", "No description")) for release in data])
        page += 1
    
    return releases

if __name__ == "__main__":
    owner = input("Enter the repository owner: ")
    repo = input("Enter the repository name: ")
    
    releases = get_github_releases(owner, repo)
    
    if not releases:
        print("No releases found or failed to fetch.")
    else:
        with open("releases.txt", "w") as file:
            for tag, desc in releases:
                file.write(f"Release: {tag}\nDescription: {desc}\n{'-'*40}\n")
        print("Releases saved to releases.txt.")
