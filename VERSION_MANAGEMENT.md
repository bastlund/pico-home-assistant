# Version Management Workflow

## Version Strategy

This project uses semantic versioning (SemVer) with automatic git information.

### Version Sources

1. **CMakeLists.txt** - Base version (0.1.0)
2. **Git describe** - Automatic build info (v0.1.0-5-g1234567)
3. **Runtime** - Available as `PROJECT_VERSION_FULL`

### Branch Workflows

#### Feature Development
```bash
# Start feature (keeps base version)
git checkout -b feature/new-sensor
# CMakeLists.txt: 0.1.0
# Runtime version: 0.1.0-feature-new-sensor-3-g1234567
```

#### Release Preparation
```bash
# Create release branch
git checkout -b release/0.2.0

# Update version in CMakeLists.txt
sed -i 's/PROJECT_VERSION_MINOR 1/PROJECT_VERSION_MINOR 2/' CMakeLists.txt
git commit -m "Bump version to 0.2.0"

# After testing and bug fixes
git checkout main
git merge release/0.2.0
git tag -a v0.2.0 -m "Release v0.2.0"
```

#### Post-Release
```bash
# Update main to next development version
git checkout main
sed -i 's/PROJECT_VERSION_PATCH 0/PROJECT_VERSION_PATCH 1/' CMakeLists.txt
git commit -m "Prepare for v0.2.1 development"
```

### Version Display in Code

Your applications can display version like this:

```c
printf("Pico W Sensor %s\n", PROJECT_VERSION_FULL);
// Output: "Pico W Sensor 0.1.0-v0.1.0-5-g1234567"
```

### Documentation Updates

- **README.md** - Update release references manually or use automation
- **DEVELOPMENT.md** - Keep workflow instructions current
- **Release notes** - Generated from git log between tags

### Automation Ideas

Consider adding these to `.github/workflows/`:

1. **Auto-update README** when tags are created
2. **Validate version** consistency in PRs
3. **Generate release notes** from commits
