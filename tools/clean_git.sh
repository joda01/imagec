git fetch --prune
for branch in $(git branch --format='%(refname:short)' | grep -vE '^\*|main|master'); do
    if ! git show-ref --verify --quiet refs/remotes/origin/$branch; then
        echo "Deleting local branch: $branch"
        git branch -D $branch
    fi
done
