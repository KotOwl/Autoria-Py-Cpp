# FILE: frontend/app.py
from flask import Flask, render_template, request, jsonify, redirect, url_for, session, send_from_directory, make_response
import requests
import os
import json
import time
from werkzeug.utils import secure_filename
from PIL import Image
import uuid

app = Flask(__name__, static_folder='static', static_url_path='/static')
app.secret_key = 'autoria-secret-key-change-in-production'
app.config['JSON_AS_ASCII'] = False  # Дозволяємо не-ASCII символи в JSON
API_BASE_URL = os.environ.get("API_BASE_URL", "http://backend:8080")

class ApiClient:
    def __init__(self, base_url):
        self.base_url = base_url
    
    def _get_headers(self, token=None):
        headers = {"Content-Type": "application/json"}
        if token:
            headers["Authorization"] = f"Bearer {token}"
        return headers
    
    def get_listings(self, search=None, brand=None, model=None, min_price=None, max_price=None, 
                     region=None, fuel_type=None, transmission=None, sort=None, order=None, 
                     page=1, per_page=10):
        try:
            params = {}
            if search:
                params['search'] = search
            if brand:
                params['brand'] = brand
            if model:
                params['model'] = model
            if min_price:
                params['min_price'] = min_price
            if max_price:
                params['max_price'] = max_price
            if region:
                params['region'] = region
            if fuel_type:
                params['fuel_type'] = fuel_type
            if transmission:
                params['transmission'] = transmission
            if sort:
                params['sort'] = sort
            if order:
                params['order'] = order
            if page:
                params['page'] = page
            if per_page:
                params['per_page'] = per_page
            
            response = requests.get(f"{self.base_url}/api/listings", params=params, timeout=5)
            return response.json() if response.status_code == 200 else []
        except Exception as e:
            logger.error(f"Get listings error: {e}")
            return []
    
    def get_listing(self, listing_id, token=None):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/listings/{listing_id}", 
                                  headers=headers, timeout=5)
            # Встановлюємо правильне кодування для відповіді
            if response.encoding is None or response.encoding.lower() != 'utf-8':
                response.encoding = 'utf-8'
            return response.json() if response.status_code == 200 else None
        except Exception as e:
            logger.error(f"Get listing error: {e}")
            return None
    
    def create_listing(self, data, token):
        try:
            headers = self._get_headers(token)
            # Конвертуємо дані в правильний формат
            json_data = {
                'brand_id': int(data.get('brand_id', 0)),
                'model_id': int(data.get('model_id', 0)),
                'year': int(data.get('year', 0)),
                'price': float(data.get('price', 0)),
                'currency': data.get('currency', 'UAH'),
                'description': data.get('description', ''),
                'region': data.get('region', ''),
                'mileage': int(data.get('mileage', 0)),
                'photos': data.get('photos', '[]'),
                'fuel_type': data.get('fuel_type', ''),
                'transmission': data.get('transmission', ''),
                'color': data.get('color', ''),
                'engine_volume': float(data.get('engine_volume', 0) or 0),
                'body_type': data.get('body_type', ''),
                'doors_count': int(data.get('doors_count', 0) or 0),
                'engine_power': int(data.get('engine_power', 0) or 0)
            }
            response = requests.post(f"{self.base_url}/api/listings", 
                                   json=json_data, headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Create listing error: {e}")
            return {"error": str(e)}
    
    def create_purchase_request(self, listing_id, message, token):
        try:
            headers = self._get_headers(token)
            response = requests.post(f"{self.base_url}/api/listings/{listing_id}/purchase",
                                   json={'message': message}, headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Create purchase request error: {e}")
            return {"error": str(e)}
    
    def mark_as_sold(self, listing_id, token):
        try:
            headers = self._get_headers(token)
            response = requests.post(f"{self.base_url}/api/listings/{listing_id}/sold",
                                   headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Mark as sold error: {e}")
            return {"error": str(e)}
    
    def update_listing(self, listing_id, data, token):
        try:
            headers = self._get_headers(token)
            json_data = {
                'brand_id': int(data.get('brand_id', 0)),
                'model_id': int(data.get('model_id', 0)),
                'year': int(data.get('year', 0)),
                'price': float(data.get('price', 0)),
                'currency': data.get('currency', 'UAH'),
                'description': data.get('description', ''),
                'region': data.get('region', ''),
                'mileage': int(data.get('mileage', 0)),
                'photos': data.get('photos', '[]'),
                'fuel_type': data.get('fuel_type', ''),
                'transmission': data.get('transmission', ''),
                'color': data.get('color', ''),
                'engine_volume': float(data.get('engine_volume', 0) or 0),
                'body_type': data.get('body_type', ''),
                'doors_count': int(data.get('doors_count', 0) or 0),
                'engine_power': int(data.get('engine_power', 0) or 0)
            }
            response = requests.put(f"{self.base_url}/api/listings/{listing_id}",
                                  json=json_data, headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Update listing error: {e}")
            return {"error": str(e)}
    
    def upload_photo(self, listing_id, photo_file, token):
        try:
            headers = {"Authorization": f"Bearer {token}"}
            files = {'photo': photo_file}
            response = requests.post(f"{self.base_url}/api/listings/{listing_id}/photos",
                                   files=files, headers=headers, timeout=30)
            return response.json()
        except Exception as e:
            logger.error(f"Upload photo error: {e}")
            return {"error": str(e)}
    
    def get_seller_stats(self, token):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/seller/stats",
                                  headers=headers, timeout=5)
            return response.json() if response.status_code == 200 else None
        except Exception as e:
            logger.error(f"Get seller stats error: {e}")
            return None
    
    def delete_listing(self, listing_id, token):
        try:
            headers = self._get_headers(token)
            response = requests.delete(f"{self.base_url}/api/listings/{listing_id}",
                                     headers=headers, timeout=10)
            if response.status_code == 204:
                return {"success": True}
            return response.json() if response.text else {"error": "Failed to delete"}
        except Exception as e:
            logger.error(f"Delete listing error: {e}")
            return {"error": str(e)}
    
    def add_to_favorites(self, listing_id, token):
        try:
            headers = self._get_headers(token)
            response = requests.post(f"{self.base_url}/api/listings/{listing_id}/favorite",
                                   headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Add to favorites error: {e}")
            return {"error": str(e)}
    
    def remove_from_favorites(self, listing_id, token):
        try:
            headers = self._get_headers(token)
            response = requests.delete(f"{self.base_url}/api/listings/{listing_id}/favorite",
                                     headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Remove from favorites error: {e}")
            return {"error": str(e)}
    
    def get_favorites(self, token):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/favorites",
                                  headers=headers, timeout=10)
            return response.json() if response.status_code == 200 else []
        except Exception as e:
            logger.error(f"Get favorites error: {e}")
            return []
    
    def add_comment(self, listing_id, comment_text, token):
        try:
            headers = self._get_headers(token)
            response = requests.post(f"{self.base_url}/api/listings/{listing_id}/comments",
                                   json={'comment_text': comment_text}, headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Add comment error: {e}")
            return {"error": str(e)}
    
    def get_comments(self, listing_id):
        try:
            response = requests.get(f"{self.base_url}/api/listings/{listing_id}/comments", timeout=10)
            return response.json() if response.status_code == 200 else []
        except Exception as e:
            logger.error(f"Get comments error: {e}")
            return []
    
    def get_notifications(self, token):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/notifications",
                                  headers=headers, timeout=10)
            return response.json() if response.status_code == 200 else []
        except Exception as e:
            logger.error(f"Get notifications error: {e}")
            return []

    def get_messages(self, token, other_user_id=None):
        try:
            headers = self._get_headers(token)
            params = {}
            if other_user_id:
                params['user_id'] = other_user_id
            response = requests.get(f"{self.base_url}/api/messages", headers=headers, params=params, timeout=10)
            return response.json() if response.status_code == 200 else []
        except Exception as e:
            logger.error(f"Get messages error: {e}")
            return []
    
    def mark_notification_read(self, notification_id, token):
        try:
            headers = self._get_headers(token)
            response = requests.post(f"{self.base_url}/api/notifications/{notification_id}/read",
                                   headers=headers, timeout=10)
            return response.json()
        except Exception as e:
            logger.error(f"Mark notification read error: {e}")
            return {"error": str(e)}
    
    def get_brands(self):
        try:
            response = requests.get(f"{self.base_url}/api/brands", timeout=5)
            return response.json() if response.status_code == 200 else []
        except:
            return []
    
    def get_models(self, brand_id):
        try:
            response = requests.get(f"{self.base_url}/api/brands/{brand_id}/models", timeout=5)
            return response.json() if response.status_code == 200 else []
        except:
            return []
    
    def login(self, email, password):
        try:
            response = requests.post(f"{self.base_url}/api/auth/login", 
                                   json={"email": email, "password": password}, timeout=10)
            if response.status_code == 200:
                return response.json()
            else:
                return {"error": f"Server error: {response.status_code}"}
        except requests.exceptions.ConnectionError:
            return {"error": "Не вдалося підключитися до сервера. Перевірте, чи запущений бекенд."}
        except requests.exceptions.Timeout:
            return {"error": "Час очікування вийшов. Спробуйте ще раз."}
        except Exception as e:
            return {"error": f"Помилка: {str(e)}"}
    
    def register(self, email, password, firstName, lastName, role):
        try:
            url = f"{self.base_url}/api/users"
            data = {"email": email, "password": password, 
                    "firstName": firstName, "lastName": lastName, "role": role}
            logger.info(f"Register request to {url} with data: {data}")
            response = requests.post(url, json=data, timeout=10)
            logger.info(f"Response status: {response.status_code}, body: {response.text}")
            if response.status_code in [200, 201]:
                return response.json()
            else:
                try:
                    error_data = response.json()
                    return {"error": error_data.get("error", f"Server error: {response.status_code}")}
                except:
                    return {"error": f"Server error: {response.status_code}"}
        except requests.exceptions.ConnectionError as e:
            logger.error(f"Connection error: {e}")
            return {"error": f"Не вдалося підключитися до сервера ({self.base_url}). Перевірте, чи запущений бекенд."}
        except requests.exceptions.Timeout:
            logger.error("Timeout error")
            return {"error": "Час очікування вийшов. Спробуйте ще раз."}
        except Exception as e:
            logger.error(f"Exception: {e}")
            return {"error": f"Помилка: {str(e)}"}
    
    def get_current_user(self, token):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/auth/me", 
                                  headers=headers, timeout=5)
            return response.json() if response.status_code == 200 else None
        except:
            return None
    
    def get_my_listings(self, token):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/listings/my", 
                                  headers=headers, timeout=5)
            if response.status_code == 200:
                try:
                    result = response.json()
                    logger.info(f"Get my listings response type: {type(result)}, length: {len(result) if isinstance(result, list) else 'not a list'}")
                    if isinstance(result, list):
                        logger.info(f"Get my listings: returning {len(result)} listings")
                        return result
                    else:
                        logger.warning(f"Get my listings: response is not a list, type: {type(result)}, value: {str(result)[:200]}")
                        return []
                except json.JSONDecodeError as e:
                    logger.error(f"Get my listings JSON decode error: {e}, response text: {response.text[:500]}")
                    return []
            else:
                logger.error(f"Get my listings error: {response.status_code} - {response.text}")
                return []
        except Exception as e:
            logger.error(f"Get my listings exception: {e}")
            return []
    
    def get_listing_details(self, listing_id, token=None):
        try:
            headers = self._get_headers(token)
            response = requests.get(f"{self.base_url}/api/listings/{listing_id}", 
                                  headers=headers, timeout=5)
            # Встановлюємо правильне кодування для відповіді
            if response.encoding is None or response.encoding.lower() != 'utf-8':
                response.encoding = 'utf-8'
            return response.json() if response.status_code == 200 else None
        except Exception as e:
            logger.error(f"Get listing details error: {e}")
            return None

api_client = ApiClient(API_BASE_URL)

# Діагностика підключення
import logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)
logger.info(f"API_BASE_URL: {API_BASE_URL}")

# Статичні файли для фото
@app.route('/static/uploads/<filename>')
def uploaded_file(filename):
    upload_dir = os.path.join(os.path.dirname(__file__), 'static', 'uploads')
    return send_from_directory(upload_dir, filename)

@app.route('/')
def index():
    # Фільтрація
    brand_filter = request.args.get('brand')
    min_price = request.args.get('min_price')
    max_price = request.args.get('max_price')
    region_filter = request.args.get('region')
    search_query = request.args.get('search', '').lower()
    sort_by = request.args.get('sort', '')
    page = int(request.args.get('page', 1))
    per_page = 10
    
    # Використовуємо новий API з фільтрацією та сортуванням на бекенді
    sort_order = request.args.get('order', 'DESC')
    sort_by_backend = 'created_at'  # Значення для бекенду
    sort_by_original = sort_by  # Оригінальне значення для шаблону
    
    if sort_by == 'price_asc':
        sort_by_backend = 'price'
        sort_order = 'ASC'
    elif sort_by == 'price_desc':
        sort_by_backend = 'price'
        sort_order = 'DESC'
    elif sort_by == 'date_asc':
        sort_by_backend = 'created_at'
        sort_order = 'ASC'
    elif sort_by == 'date_desc':
        sort_by_backend = 'created_at'
        sort_order = 'DESC'
    elif sort_by == 'mileage_asc':
        sort_by_backend = 'mileage'
        sort_order = 'ASC'
    elif sort_by == 'mileage_desc':
        sort_by_backend = 'mileage'
        sort_order = 'DESC'
    elif sort_by == 'views_desc':
        sort_by_backend = 'view_count'
        sort_order = 'DESC'
    else:
        sort_by_backend = 'created_at'
        sort_order = 'DESC'
    
    listings = api_client.get_listings(
        search=search_query if search_query else None,
        brand=brand_filter if brand_filter else None,
        min_price=float(min_price) if min_price else None,
        max_price=float(max_price) if max_price else None,
        region=region_filter if region_filter else None,
        fuel_type=request.args.get('fuel_type') if request.args.get('fuel_type') else None,
        transmission=request.args.get('transmission') if request.args.get('transmission') else None,
        sort=sort_by_backend,
        order=sort_order,
        page=page,
        per_page=per_page
    )
    
    user = None
    if 'token' in session:
        user = api_client.get_current_user(session['token'])
    
    # Додаємо назви марок та моделей до оголошень, а також парсимо photos
    brands = api_client.get_brands()
    filtered_listings = []
    for listing in listings:
        brand_id = listing.get('brandId')
        model_id = listing.get('modelId')
        
        for brand in brands:
            if brand.get('id') == brand_id:
                listing['brandName'] = brand.get('name')
                models = api_client.get_models(brand_id)
                for m in models:
                    if m.get('id') == model_id:
                        listing['modelName'] = m.get('name')
                        break
                break
        
        # Парсимо photos для кожного оголошення
        photos_data = listing.get('photos')
        photos = []
        if photos_data:
            try:
                if isinstance(photos_data, list):
                    photos = photos_data
                elif isinstance(photos_data, str) and photos_data.strip() and photos_data != '[]':
                    photos = json.loads(photos_data)
            except Exception as e:
                logger.error(f"Error parsing photos for listing {listing.get('id')}: {e}")
                photos = []
        listing['photos'] = [p for p in photos if p and isinstance(p, str) and not p.startswith('placeholder_') and p.strip()]
        filtered_listings.append(listing)
    
    # Бекенд вже зробив сортування та пагінацію
    # Оцінюємо загальну кількість сторінок на основі кількості результатів
    # (якщо результатів менше за per_page, то це остання сторінка)
    total_pages = page if len(filtered_listings) == per_page else page
    
    return render_template('index.html', listings=filtered_listings, user=user, 
                         brands=brands, page=page, total_pages=total_pages,
                         brand_filter=brand_filter, min_price=min_price, 
                         max_price=max_price, region_filter=region_filter,
                         search_query=search_query, sort_by=sort_by_original)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        email = request.form.get('email')
        password = request.form.get('password')
        
        result = api_client.login(email, password)
        if result and 'token' in result:
            session['token'] = result['token']
            session['user'] = result.get('user', {})
            return redirect(url_for('index'))
        else:
            error = result.get('error', 'Невірний email або пароль') if result else 'Помилка з\'єднання'
            return render_template('login.html', error=error)
    
    return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        email = request.form.get('email')
        password = request.form.get('password')
        firstName = request.form.get('firstName')
        lastName = request.form.get('lastName')
        role = request.form.get('role', 'buyer')
        
        result = api_client.register(email, password, firstName, lastName, role)
        if result and 'token' in result and 'error' not in result:
            session['token'] = result['token']
            # Отримуємо дані користувача
            user = api_client.get_current_user(result['token'])
            if user:
                session['user'] = user
            return redirect(url_for('index'))
        else:
            error = result.get('error', 'Помилка реєстрації') if result else 'Помилка з\'єднання з сервером'
            return render_template('register.html', error=error, brands=api_client.get_brands())
    
    return render_template('register.html', brands=api_client.get_brands())

@app.route('/profile')
def profile():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    # Отримуємо оголошення користувача
    listings = api_client.get_my_listings(session['token'])
    print(f"DEBUG Profile: listings type = {type(listings)}, count = {len(listings) if listings else 0}")
    logger.info(f"Profile: listings type = {type(listings)}, count = {len(listings) if listings else 0}")
    
    if not listings:
        print("DEBUG Profile: listings is empty or None")
        listings = []
    else:
        print(f"DEBUG Profile: first listing = {listings[0] if len(listings) > 0 else 'N/A'}")
    
    brands = api_client.get_brands()
    
    # Додаємо назви марок та моделей до оголошень
    for listing in listings:
        if not isinstance(listing, dict):
            continue
        brand_id = listing.get('brandId')
        model_id = listing.get('modelId')
        
        for brand in brands:
            if brand.get('id') == brand_id:
                listing['brandName'] = brand.get('name')
                models = api_client.get_models(brand_id)
                for m in models:
                    if m.get('id') == model_id:
                        listing['modelName'] = m.get('name')
                        break
                break
    
    logger.info(f"Profile: returning {len(listings)} listings to template")
    return render_template('profile.html', user=user, listings=listings)

@app.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('index'))

@app.route('/listing/<int:listing_id>')
def listing_details(listing_id):
    token = session.get('token')
    listing = api_client.get_listing_details(listing_id, token)
    if not listing:
        return "Оголошення не знайдено", 404
    
    # Отримуємо марку та модель
    brand = None
    model = None
    if listing.get('brandId'):
        brands = api_client.get_brands()
        for b in brands:
            if b.get('id') == listing.get('brandId'):
                brand = b.get('name')
                break
    
    if listing.get('modelId') and brand:
        models = api_client.get_models(listing.get('brandId'))
        for m in models:
            if m.get('id') == listing.get('modelId'):
                model = m.get('name')
                break
    
    user = None
    is_owner = False
    if token:
        user = api_client.get_current_user(token)
        if user and listing.get('sellerId') == user.get('id'):
            is_owner = True
    
    # Парсимо photos якщо вони є
    photos_data = listing.get('photos')
    photos = []
    
    if photos_data:
        try:
            # Якщо photos вже є списком, використовуємо його
            if isinstance(photos_data, list):
                photos = photos_data
            # Якщо photos є рядком JSON, парсимо його
            elif isinstance(photos_data, str):
                if photos_data and photos_data.strip() and photos_data != '[]':
                    photos = json.loads(photos_data)
                else:
                    photos = []
        except json.JSONDecodeError as e:
            logger.error(f"Error parsing photos JSON: {e}, photos_data: {photos_data}")
            photos = []
        except Exception as e:
            logger.error(f"Error parsing photos: {e}, photos_data: {photos_data}")
            photos = []
    
    # Перевіряємо, чи фото не є placeholder та не порожні
    photos = [p for p in photos if p and isinstance(p, str) and not p.startswith('placeholder_') and p.strip()]
    listing['photos'] = photos
    
    logger.debug(f"Listing {listing_id} photos: {photos}")
    
    # Завантажуємо коментарі
    comments = api_client.get_comments(listing_id)
    
    # Переконаємося, що region правильно обробляється
    if listing.get('region'):
        # Переконуємося, що region є правильною UTF-8 строкою
        region = listing.get('region')
        if isinstance(region, str):
            # Перевіряємо, чи правильно декодується
            try:
                region.encode('utf-8').decode('utf-8')
                listing['region'] = region  # Залишаємо як є
            except (UnicodeEncodeError, UnicodeDecodeError):
                # Якщо є проблема, намагаємося виправити
                listing['region'] = region.encode('latin-1', errors='ignore').decode('utf-8', errors='ignore')
    
    html = render_template('listing_details.html', listing=listing, brand=brand, 
                             model=model, user=user, is_owner=is_owner, comments=comments)
    return make_response(html, 200, {'Content-Type': 'text/html; charset=utf-8'})

@app.route('/listing/<int:listing_id>/purchase', methods=['POST'])
def purchase_listing(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    message = request.form.get('message', '')
    result = api_client.create_purchase_request(listing_id, message, session['token'])
    
    if result and 'success' in result:
        return redirect(url_for('listing_details', listing_id=listing_id))
    else:
        error = result.get('error', 'Помилка') if result else 'Помилка з\'єднання'
        return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/listing/<int:listing_id>/sold', methods=['POST'])
def mark_listing_as_sold(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    result = api_client.mark_as_sold(listing_id, session['token'])
    
    if result and 'success' in result:
        return redirect(url_for('listing_details', listing_id=listing_id))
    else:
        return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/listing/<int:listing_id>/edit', methods=['GET', 'POST'])
def edit_listing(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    # Перевіряємо, чи користувач є власником
    listing = api_client.get_listing_details(listing_id, session['token'])
    if not listing or listing.get('sellerId') != user.get('id'):
        return redirect(url_for('listing_details', listing_id=listing_id))
    
    if request.method == 'POST':
        data = {
            'brand_id': request.form.get('brand_id'),
            'model_id': request.form.get('model_id'),
            'year': request.form.get('year'),
            'price': request.form.get('price'),
            'currency': request.form.get('currency'),
            'description': request.form.get('description'),
            'region': request.form.get('region'),
            'mileage': request.form.get('mileage'),
            'photos': listing.get('photos', '[]'),
            'fuel_type': request.form.get('fuel_type', ''),
            'transmission': request.form.get('transmission', ''),
            'color': request.form.get('color', ''),
            'engine_volume': request.form.get('engine_volume', '0'),
            'body_type': request.form.get('body_type', ''),
            'doors_count': request.form.get('doors_count', '0'),
            'engine_power': request.form.get('engine_power', '0')
        }
        
        result = api_client.update_listing(listing_id, data, session['token'])
        if result and 'success' in result:
            return redirect(url_for('listing_details', listing_id=listing_id))
        else:
            error = result.get('error', 'Помилка оновлення') if result else 'Помилка з\'єднання'
            brands = api_client.get_brands()
            return render_template('edit_listing.html', listing=listing, error=error, brands=brands, user=user)
    
    brands = api_client.get_brands()
    return render_template('edit_listing.html', listing=listing, brands=brands, user=user)

@app.route('/listing/<int:listing_id>/delete', methods=['POST'])
def delete_listing(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    result = api_client.delete_listing(listing_id, session['token'])
    
    if result and 'success' in result:
        return redirect(url_for('my_listings'))
    else:
        return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/listing/<int:listing_id>/favorite', methods=['POST'])
def toggle_favorite(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    result = api_client.add_to_favorites(listing_id, session['token'])
    logger.info(f"Add to favorites result: {result}")
    # Перенаправляємо незалежно від результату (можна додати flash message)
    return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/listing/<int:listing_id>/comment', methods=['POST'])
def add_comment(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    comment_text = request.form.get('comment_text', '')
    if comment_text:
        result = api_client.add_comment(listing_id, comment_text, session['token'])
    
    return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/favorites')
def favorites():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    listings = api_client.get_favorites(session['token'])
    user = api_client.get_current_user(session['token'])
    brands = api_client.get_brands()
    
    # Додаємо назви марок та моделей
    for listing in listings:
        brand_id = listing.get('brandId')
        model_id = listing.get('modelId')
        
        for brand in brands:
            if brand.get('id') == brand_id:
                listing['brandName'] = brand.get('name')
                models = api_client.get_models(brand_id)
                for m in models:
                    if m.get('id') == model_id:
                        listing['modelName'] = m.get('name')
                        break
                break
    
    return render_template('favorites.html', listings=listings, user=user, brands=brands)

@app.route('/notifications')
def notifications():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    notifications_list = api_client.get_notifications(session['token'])
    user = api_client.get_current_user(session['token'])
    
    return render_template('notifications.html', notifications=notifications_list, user=user)

@app.route('/messages')
def messages():
    if 'token' not in session:
        return redirect(url_for('login'))
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    other_user_id = request.args.get('user_id')
    try:
        other_user_id = int(other_user_id) if other_user_id else None
    except:
        other_user_id = None
    msgs = api_client.get_messages(session['token'], other_user_id)
    return render_template('messages.html', messages=msgs, user=user)

@app.route('/create-listing', methods=['GET', 'POST'])
def create_listing():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    if request.method == 'POST':
        # Обробка форми створення оголошення
        photos = []
        # Завантаження фото на сервер
        if 'photos' in request.files:
            files = request.files.getlist('photos')
            upload_dir = os.path.join(os.path.dirname(__file__), 'static', 'uploads')
            os.makedirs(upload_dir, exist_ok=True)
            
            for file in files:
                if file.filename:
                    # Перевіряємо розширення файлу
                    allowed_extensions = {'png', 'jpg', 'jpeg', 'gif', 'webp'}
                    file_ext = file.filename.rsplit('.', 1)[1].lower() if '.' in file.filename else ''
                    
                    if file_ext in allowed_extensions:
                        # Генеруємо унікальне ім'я файлу
                        filename = secure_filename(file.filename)
                        # Додаємо UUID для унікальності
                        unique_id = str(uuid.uuid4())[:8]
                        base_name = os.path.splitext(filename)[0]
                        unique_filename = f"{int(time.time() * 1000)}_{unique_id}_{base_name}.{file_ext}"
                        filepath = os.path.join(upload_dir, unique_filename)
                        
                        try:
                            # Відкриваємо зображення
                            img = Image.open(file.stream)
                            
                            # Конвертуємо в RGB якщо потрібно (для PNG з прозорістю)
                            if img.mode in ('RGBA', 'LA', 'P'):
                                background = Image.new('RGB', img.size, (255, 255, 255))
                                if img.mode == 'P':
                                    img = img.convert('RGBA')
                                background.paste(img, mask=img.split()[-1] if img.mode == 'RGBA' else None)
                                img = background
                            elif img.mode != 'RGB':
                                img = img.convert('RGB')
                            
                            # Оптимізація: зменшуємо розмір якщо він занадто великий
                            max_size = (1920, 1920)  # Максимальний розмір
                            if img.size[0] > max_size[0] or img.size[1] > max_size[1]:
                                img.thumbnail(max_size, Image.Resampling.LANCZOS)
                            
                            # Зберігаємо оптимізоване зображення
                            img.save(filepath, 'JPEG', quality=85, optimize=True)
                            
                            # Створюємо прев'ю (thumbnail)
                            preview_size = (300, 300)
                            preview_img = img.copy()
                            preview_img.thumbnail(preview_size, Image.Resampling.LANCZOS)
                            preview_filename = f"preview_{unique_filename}"
                            preview_path = os.path.join(upload_dir, preview_filename)
                            preview_img.save(preview_path, 'JPEG', quality=80, optimize=True)
                            
                            # Зберігаємо відносний шлях для відображення
                            photos.append(f"/static/uploads/{unique_filename}")
                            logger.info(f"File saved and optimized: {unique_filename}")
                        except Exception as e:
                            logger.error(f"Error processing file {filename}: {e}")
        
        data = {
            'brand_id': request.form.get('brand_id'),
            'model_id': request.form.get('model_id'),
            'year': request.form.get('year'),
            'price': request.form.get('price'),
            'currency': request.form.get('currency'),
            'description': request.form.get('description'),
            'region': request.form.get('region'),
            'mileage': request.form.get('mileage'),
            'photos': json.dumps(photos) if photos else '[]',
            'fuel_type': request.form.get('fuel_type', ''),
            'transmission': request.form.get('transmission', ''),
            'color': request.form.get('color', ''),
            'engine_volume': request.form.get('engine_volume', '0'),
            'body_type': request.form.get('body_type', ''),
            'doors_count': request.form.get('doors_count', '0'),
            'engine_power': request.form.get('engine_power', '0')
        }
        
        result = api_client.create_listing(data, session['token'])
        logger.info(f"Create listing result: {result}")
        if result and (result.get('success') or 'success' in str(result)):
            # Отримуємо ID створеного оголошення
            listing_id = result.get('id')
            if listing_id:
                return redirect(url_for('listing_details', listing_id=listing_id))
            return redirect(url_for('my_listings'))
        else:
            error = result.get('error', 'Помилка створення оголошення') if result else 'Помилка з\'єднання'
            logger.error(f"Create listing error: {error}")
            brands = api_client.get_brands()
            return render_template('create_listing.html', error=error, brands=brands, user=user)
    
    brands = api_client.get_brands()
    return render_template('create_listing.html', brands=brands, user=user)

@app.route('/my-listings')
def my_listings():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    listings = api_client.get_my_listings(session['token'])
    logger.info(f"My listings count: {len(listings) if listings else 0}")
    
    if not listings:
        logger.warning("No listings returned from API")
        listings = []
    
    brands = api_client.get_brands()
    
    # Додаємо назви марок та моделей до оголошень
    for listing in listings:
        if not isinstance(listing, dict):
            continue
        brand_id = listing.get('brandId')
        model_id = listing.get('modelId')
        
        for brand in brands:
            if brand.get('id') == brand_id:
                listing['brandName'] = brand.get('name')
                models = api_client.get_models(brand_id)
                for m in models:
                    if m.get('id') == model_id:
                        listing['modelName'] = m.get('name')
                        break
                break
    
    # Фільтрація на сервері
    filtered_listings = listings
    if request.args.get('brand'):
        filtered_listings = [l for l in filtered_listings if str(l.get('brandId')) == request.args.get('brand')]
    if request.args.get('status'):
        filtered_listings = [l for l in filtered_listings if l.get('status') == request.args.get('status')]
    if request.args.get('min_price'):
        try:
            min_price = float(request.args.get('min_price'))
            filtered_listings = [l for l in filtered_listings if l.get('price', 0) >= min_price]
        except:
            pass
    if request.args.get('max_price'):
        try:
            max_price = float(request.args.get('max_price'))
            filtered_listings = [l for l in filtered_listings if l.get('price', 0) <= max_price]
        except:
            pass
    
    return render_template('my_listings.html', listings=filtered_listings, user=user, brands=brands)

@app.route('/api/listings', methods=['GET'])
def get_listings():
    listings = api_client.get_listings()
    return jsonify(listings)

@app.route('/api/brands', methods=['GET'])
def get_brands():
    brands = api_client.get_brands()
    return jsonify(brands)

@app.route('/api/brands/<int:brand_id>/models', methods=['GET'])
def get_models(brand_id):
    models = api_client.get_models(brand_id)
    return jsonify(models)

@app.route('/listing/<int:listing_id>/upload-photo', methods=['POST'])
def upload_photo(listing_id):
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    if 'photo' not in request.files:
        return redirect(url_for('listing_details', listing_id=listing_id))
    
    photo = request.files['photo']
    if photo.filename == '':
        return redirect(url_for('listing_details', listing_id=listing_id))
    
    # Завантажуємо фото через API
    photo.seek(0)  # Reset file pointer
    photo_data = photo.read()
    photo.seek(0)  # Reset again for reading
    result = api_client.upload_photo(listing_id, (photo.filename, photo_data, photo.content_type), session['token'])
    
    if result and result.get('success'):
        return redirect(url_for('listing_details', listing_id=listing_id))
    else:
        error = result.get('error', 'Помилка завантаження фото') if result else 'Помилка з\'єднання'
        logger.error(f"Upload photo error: {error}")
        return redirect(url_for('listing_details', listing_id=listing_id))

@app.route('/seller/stats')
def seller_stats():
    if 'token' not in session:
        return redirect(url_for('login'))
    
    user = api_client.get_current_user(session['token'])
    if not user:
        session.clear()
        return redirect(url_for('login'))
    
    stats = api_client.get_seller_stats(session['token'])
    if not stats:
        stats = {
            'totalListings': 0,
            'activeListings': 0,
            'soldListings': 0,
            'totalViews': 0,
            'averagePrice': 0,
            'viewsByDay': [],
            'popularListings': []
        }
    else:
        # Перетворюємо viewsByDay на список словників для шаблону
        if 'viewsByDay' in stats and isinstance(stats['viewsByDay'], list):
            views_by_day = []
            for item in stats['viewsByDay']:
                if isinstance(item, dict):
                    views_by_day.append(item)
                elif isinstance(item, (list, tuple)) and len(item) >= 2:
                    views_by_day.append({'date': item[0], 'views': item[1]})
            stats['viewsByDay'] = views_by_day
        
        # Перетворюємо popularListings на список словників для шаблону
        if 'popularListings' in stats and isinstance(stats['popularListings'], list):
            popular_listings = []
            for item in stats['popularListings']:
                if isinstance(item, dict):
                    popular_listings.append(item)
                elif isinstance(item, (list, tuple)) and len(item) >= 2:
                    popular_listings.append({'name': item[0], 'views': item[1]})
            stats['popularListings'] = popular_listings
    
    return render_template('seller_stats.html', user=user, stats=stats)

@app.route('/api/auth/login', methods=['POST'])
def api_login():
    data = request.get_json()
    try:
        response = requests.post(f"{API_BASE_URL}/api/auth/login", 
                              json=data, timeout=5)
        return jsonify(response.json()), response.status_code
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/api/auth/me', methods=['GET'])
def get_current_user_api():
    token = request.headers.get('Authorization', '').replace('Bearer ', '')
    if not token:
        return jsonify({"error": "Unauthorized"}), 401
    try:
        headers = {"Authorization": f"Bearer {token}"}
        response = requests.get(f"{API_BASE_URL}/api/auth/me", 
                              headers=headers, timeout=5)
        return jsonify(response.json()), response.status_code
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.after_request
def after_request(response):
    # Встановлюємо правильний Content-Type з charset для всіх відповідей
    if response.content_type:
        if 'text/html' in response.content_type:
            response.headers['Content-Type'] = 'text/html; charset=utf-8'
        elif 'application/json' in response.content_type:
            response.headers['Content-Type'] = 'application/json; charset=utf-8'
    # Додатково встановлюємо заголовок для гарантії правильного кодування
    response.headers['X-Content-Type-Options'] = 'nosniff'
    return response

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
