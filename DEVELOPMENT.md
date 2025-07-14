# Development Workflow

## Branch Strategy

We use GitHub Flow with release branches for this project.

### Main Branches

- `main` - Stable code, always deployable
- `release/x.y.z` - Prepare releases, bug fixes only

### Feature Branches

- `feature/description` - New features
- `bugfix/description` - Bug fixes
- `hotfix/description` - Critical fixes

### Workflow

1. **Start new feature:**
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feature/your-feature-name
   ```

2. **Develop and commit:**
   ```bash
   git add .
   git commit -m "descriptive commit message"
   git push origin feature/your-feature-name
   ```

3. **Create Pull Request:**
   - Target: `main` branch
   - Review and test before merge

4. **Release process:**
   ```bash
   # Create release branch
   git checkout -b release/0.2.0
   
   # Update version in CMakeLists.txt
   # Test and fix bugs
   
   # Merge to main and tag
   git checkout main
   git merge release/0.2.0
   git tag -a v0.2.0 -m "Release v0.2.0"
   git push origin main --tags
   ```

### Branch Protection

- `main` branch requires Pull Request reviews
- No direct commits to `main`
- All tests must pass before merge

## Debug Configuration

### Setting Debug Levels

For development, you can adjust debug output verbosity:

**Via cmake-tools-kits.json (VS Code users):**
```json
{
  "cmakeSettings": {
    "DEBUG_LEVEL": "3"
  }
}
```

**Via CMake command line:**
```bash
cmake -DDEBUG_LEVEL=3 ..
```

### Debug Levels
- `0` - Production: Errors only
- `1` - Warnings: Errors + Warnings  
- `2` - Info: Errors + Warnings + Info (default)
- `3` - Debug: Errors + Warnings + Info + Debug
- `4` - Verbose: All messages including detailed traces

### Development Recommendations
- Use `DEBUG_LEVEL=3` for active development
- Use `DEBUG_LEVEL=4` for troubleshooting network/MQTT issues
- Use `DEBUG_LEVEL=0` for production builds
- Use `DEBUG_LEVEL=2` for general testing (default)
