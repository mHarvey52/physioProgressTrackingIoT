Rails.application.routes.draw do
  root 'data#index'
  get '/data', to: 'data#index'
  # For details on the DSL available within this file, see https://guides.rubyonrails.org/routing.html
end
